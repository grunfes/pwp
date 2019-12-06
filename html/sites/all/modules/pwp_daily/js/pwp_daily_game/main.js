/* global jQuery, _, axios, Drupal, Vue, console */

;(function ($, _, axios, Drupal, Vue) {
  Drupal.behaviors.dailyGame = {
    components: [
      {
        name: 'App',
        component: {
          template: '#app-template',
          data: function () {
            return {
              show: undefined,
              loading: true,
              error: undefined,
              picks: {},
              component: 'game-intro'
            };
          },
          computed: {
            hasDailyGame: function () {
              return !_.isEmpty(this.show);
            }
          },
          mounted: function () {
            axios.get('/pwp_daily/data/fetch/')
            .then(function (response) {
              var responseData = response.data;

              if (response.status === 200 && responseData.status === 200) {
                this.show = responseData.data;
                this.loading = false;
              }
            }.bind(this));
          },
          methods: {
            handleGameStart: function () {
              this.component = 'game';
            },
            handleGameEnd: function (picks) {
              this.picks = picks;
              this.component = 'game-summary';
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
            this.$__interval = setInterval(this.update,  this.interval);
          },
          beforeDestroy: function () {
            clearInterval(this.$__interval);
          },
          methods: {
            update: function () {
              this.elapsed -= this.interval;
              if (this.elapsed <= 0) {
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
            handleComplete: function () {
              this.intervalEnded = true;
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
              picks: {}
            };
          },
          computed: {
            match: function () {
              return this.matches[this.index];
            }
          },
          methods: {
            handleChange: function () {
              if (this.index < this.matches.length - 1) {
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
              selected: undefined
            };
          },
          methods: {
            shouldAppend: function (index) {
              return index !== Object.keys(this.teams).length - 1;
            },
            handleTeamInput: function (team) {
              this.$emit('pick', { match: this.id, team: team });
            }
          }
        }
      },

      {
        name: 'Team',
        component: {
          template: '#team-template',
          inheritAttrs: false,
          props:{
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
          methods: {
            handleInput: function (e) {
              this.$emit('input', e.target.checked ? this.id : undefined);
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
            picks: {
              type: Object,
              default: function () { return {}; }
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
