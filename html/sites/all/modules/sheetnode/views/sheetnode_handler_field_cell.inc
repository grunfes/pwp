<?php

/**
 * @file
 * Views handler for sheetnode cells.
 */

/**
 * Provides a custom sheetnode handler.
 */
class sheetnode_handler_field_cell extends views_handler_field {

  /**
   * Set options for views handler.
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['reference'] = array('default' => '');
    return $options;
  }

  /**
   * Set options form for views handler.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['reference'] = array(
      '#type' => 'textfield',
      '#title' => t('Cell reference'),
      '#default_value' => $this->options['reference'],
      '#description' => t('Use a cell coordinate such as A1 to refer to a specific cell.'),
    );
  }

  /**
   * Render the views field.
   */
  function render($values) {
    module_load_include('inc', 'sheetnode', 'socialcalc');
    $sheet = $values->{$this->field_alias};
    if (empty($sheet)) {
      return NULL;
    }
    $sc = socialcalc_parse_values($sheet);
    return isset($sc['cells'][$this->options['reference']]) ? $sc['cells'][$this->options['reference']]['datavalue'] : NULL;
  }

}
