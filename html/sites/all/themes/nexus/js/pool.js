/* global Drupal */

;(function ($, Drupal) {
  Drupal.behaviors.grunfesPool = {
    attach: function (context) {
      $('form', context).once('tt').each(function () {
        var $el = $(this);
        if (/^\/(pool|my_picks|mitb)/.test($el.attr('action'))) {
          $el.submit(function () {
            $(this).find('input[type="submit"]').attr('disabled', 'disabled');
          });
        }
      });
    }
  };
})(jQuery, Drupal);
