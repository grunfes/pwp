<?php

/**
 * Class pwp_daily_helpers
 */
class pwp_daily_helpers {

  /**
   * @param $element
   * @param $formState
   *
   * @return array
   */
  public static function getInlineEntityFormState($element, $formState) {
    return $formState['inline_entity_form'][$element['#ief_id']];
  }

  /**
   * @param string $key
   *
   * @return string
   */
  public static function getTeamFieldName($key) {
    return "field_team_{$key}_daily";
  }

  /**
   * @param $path
   *
   * @return string
   */
  public static function getPath($path) {
    return drupal_get_path('module', 'pwp_daily') . '/' . $path;
  }

  /**
   * @return array
   */
  public static function getSharedJSLibraries() {
    return [
      [
        'type' => 'file',
        'data' => static::getPath('js/lib/axios.min.js'),
        'preprocess' => FALSE,
      ],
      [
        'type' => 'file',
        'data' => static::getPath('js/lib/lodash.min.js'),
        'preprocess' => FALSE,
      ],
      [
        'type' => 'file',
        'data' => static::getPath('js/lib/anime.min.js'),
        'preprocess' => FALSE,
      ],
      [
        'type' => 'file',
        'data' => static::getPath('js/lib/vue.js'),
        'preprocess' => FALSE,
      ],
    ];
  }

  /**
   * @param $data
   *
   * @return string
   */
  public static function dailyPickTitleGenerator($data) {
    return implode('-', array_filter([
      $data['user'],
      $data['show'],
      $data['match'],
      $data['team'],
    ]));
  }

}
