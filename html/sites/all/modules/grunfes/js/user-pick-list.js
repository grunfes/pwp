/* global Drupal */

;(function ($, Drupal) {
  Drupal.behaviors.grunfes = {
    attach: function (context) {
      $("#block-grunfes-list-of-user-picks", context).once("accordion", function () {
        var $root = $(this);

        jQuery($root.find(".accordion")).accordion({ active: false, collapsible: true });
      });
    }
  };
})(jQuery, Drupal);
