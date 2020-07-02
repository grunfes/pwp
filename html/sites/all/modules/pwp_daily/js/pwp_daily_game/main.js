/* global jQuery, _, axios, anime, Drupal, Vue */

;(function ($, _, axios, Drupal, Vue) {
  Drupal.behaviors.dailyGame = {
    components: [
      {
        name: 'App',
        component: {
          template: '#app-template',
          data: function () {
            return {
              gameData: {},
              loading: true,
              error: undefined,
              summary: {},
              component: 'game-intro'
            };
          },
          computed: {
            hasDailyGame: function () {
              return this.gameData.hasDailyGame;
            },

            hasUserPlayed: function () {
              return this.gameData.hasUserPlayed;
            }
          },
          mounted: function () {
            axios.get('/pwp_daily/data/fetch')
            .then(function (response) {
              var responseData = response.data;

              if (response.status === 200 && responseData.status === 200) {
                this.gameData = responseData.data;
                this.loading = false;
              }
            }.bind(this))
            .catch(function (error) {
              this.error = error;
              this.loading = false;
            }.bind(this));
          },
          methods: {
            handleGameStart: function () {
              this.component = 'game';
            },
            handleGameEnd: function (picks) {
              var picksData = _.chain(_.keys(picks))
              .filter(function (matchId) {
                return picks[matchId] !== undefined;
              })
              .map(function (matchId) {
                return { match: matchId, team: picks[matchId] };
              })
              .value();

              axios.post('/pwp_daily/data/save', {
                show: this.gameData.show.id,
                picks: picksData
              }, {
                headers: {
                  'Content-Type': 'application/json'
                }
              })
              .then(function (response) {
                var responseData = response.data;
                if (response.status === 200 && responseData.status === 200) {
                  this.summary = responseData.data;
                  this.component = 'game-summary';
                }
              }.bind(this))
              .catch(function (error) {
                this.error = error;
              }.bind(this));
            }
          }
        }
      },

      {
        name: 'Interval',
        component: {
          inheritAttrs: false,
          props: {
            duration: {
              type: Number,
              required: true
            },
            interval: {
              type: Number,
              default: 1000
            }
          },
          data: function () {
            return {
              elapsed: this.duration * 1000
            };
          },
          mounted: function () {
            this.$__interval = setInterval(this.update, this.interval);
          },
          beforeDestroy: function () {
            clearInterval(this.$__interval);
          },
          methods: {
            update: function () {
              this.elapsed -= this.interval;
              if (this.elapsed < 0) {
                clearInterval(this.$__interval);
                this.$emit('complete');
              }
            }
          },
          render: function () {
            return this.$scopedSlots.default({
              elapsed: this.elapsed
            });
          }
        }
      },

      {
        name: 'Countdown',
        component: {
          template: '#countdown-template',
          inheritAttrs: false,
          props: {
            duration: {
              type: Number,
              required: true
            }
          },
          data: function () {
            return {
              intervalEnded: false
            };
          },
          methods: {
            get: function (value) {
              if (value !== 0) {
                return value / 1000;
              }

              return 'RUMBLE';
            },
            handleComplete: function () {
              this.$emit('complete');
            }
          }
        }
      },

      {
        name: 'CountdownAnimation',
        component: {
          template: '#countdown-animation-template',
          props: {
            countdown: {
              type: [String, Number],
              required: true
            }
          },
          watch: {
            countdown: function () {
              anime({
                targets: this.$refs.countdownEl,
                duration: 900,
                scale: [3, 1]
              });
            }
          }
        }
      },

      {
        name: 'ProgressBar',
        component: {
          template: '#progress-bar-template',
          props: {
            duration: {
              type: Number,
              required: true
            },
            progress: {
              type: Number,
              required: true
            }
          },
          computed: {
            style: function () {
              return {
                width: this.progress * 100 / this.duration + '%'
              };
            }
          }
        }
      },

      {
        name: 'GameIntro',
        component: {
          template: '#game-intro-template',
          inheritAttrs: false,
          props: {
            show: {
              type: Object,
              required: true
            }
          }
        }
      },

      {
        name: 'Game',
        component: {
          template: '#game-template',
          inheritAttrs: false,
          props: {
            matches: {
              type: Array,
              required: true
            }
          },
          data: function () {
            return {
              index: 0,
              picks: {},
              isCountdown: true
            };
          },
          computed: {
            match: function () {
              return this.matches[this.index];
            }
          },
          methods: {
            handleCountdownComplete: function () {
              this.isCountdown = false;
            },

            handleChange: function () {
              if (this.index < this.matches.length - 1) {
                this.isCountdown = true;
                this.index += 1;
                return;
              }

              this.$emit('game-end', this.picks);
            },

            handleTeamPick: function (pick) {
              this.picks[pick.match] = pick.team;
            }
          }
        }
      },

      {
        name: 'Match',
        component: {
          template: '#match-template',
          inheritAttrs: false,
          props: {
            id: {
              type: String,
              required: true
            },

            title: {
              type: String,
              default: ''
            },

            teams: {
              type: Array,
              required: true
            }
          },
          data: function () {
            return {
              selected: undefined,
              noContestValue: '-1'
            };
          },
          computed: {
            isNoContest: function () {
              return this.selected === this.noContestValue;
            }
          },
          methods: {
            shouldAppend: function (index) {
              return index !== Object.keys(this.teams).length - 1;
            },
            handleTeamInput: function (team) {
              this.$emit('pick', { match: this.id, team: team });
            },
            handleNoContestChange: function (e) {
              this.selected = e.target.checked ? this.noContestValue : undefined;

              this.$emit('pick', { match: this.id, team: this.selected });
            }
          }
        }
      },

      {
        name: 'Team',
        component: {
          template: '#team-template',
          inheritAttrs: false,
          props: {
            id: {
              type: String,
              required: true
            },

            checked: {
              type: Boolean,
              default: false
            },

            wrestlers: {
              type: Array,
              required: true
            }
          },
          computed: {
            checkedCls: function () {
              return [
                { 'dailyGameTeam__checked': this.checked }
              ];
            }
          },
          methods: {
            handleInput: function () {
              this.$emit('input', !this.checked ? this.id : undefined);
            }
          }
        }
      },

      {
        name: 'GameSummary',
        component: {
          template: '#game-summary-template',
          inheritAttrs: false,
          props: {
            summary: {
              type: Object,
              required: true
            },
            user: {
              type: Object,
              required: true
            }
          },
          computed: {
            klass: function () {
              return {
                'dailyGameSummary--perfect': this.summary.perfectGame || false
              };
            }
          }
        }
      }
    ],
    registerComponents: function () {
      _.each(this.components, function (val) {
        Vue.component(val.name, val.component);
      });
    },

    attach: function (context) {
      var rootEl = '#daily_game';
      $(rootEl, context).once('daily_game', function () {
        this.registerComponents();

        new Vue({
          el: rootEl,
          template: '<app />'
        });
      }.bind(this));
    }
  };
})(jQuery, _, axios, Drupal, Vue);
