<?php

/**
 * Class pwp_daily_helpers
 */
class pwp_daily_helpers {

  /**
   * @param string $key
   *
   * @return string
   */
  public static function getTeamFieldName($key) {
    return "field_team_{$key}_daily";
  }

}
