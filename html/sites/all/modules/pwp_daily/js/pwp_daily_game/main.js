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
              component: 'game-intro'
            };
          },
          mounted: function () {
            axios.get('/pwp_daily/data/fetch/')
            .then(function (data) {
              if (data.status === 200 && data.data.status === 200) {
                this.show = data.data.data;
                this.loading = false;
              }
            }.bind(this));
          },
          methods: {
            handleGameStart: function () {
              this.component = 'game';
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
              type: Object,
              required: true
            }
          },
          data: function () {
            return {
              selected: undefined
            };
          },
          methods: {
            getKey: function (key) {
              return this.id + '_' +  key;
            },

            shouldAppend: function (index) {
              return index !== Object.keys(this.teams).length - 1;
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

            team: {
              type: String,
              required: true
            },

            wrestlers: {
              type: Array,
              default: function () { return []; }
            }
          },
          methods: {
            handleInput: function (e) {
              var key = this.id + '_' + this.team;
              this.$emit('input', e.target.checked ? key : undefined);
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
