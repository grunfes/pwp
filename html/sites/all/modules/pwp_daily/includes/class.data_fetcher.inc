<?php

/**
 * Class DailyDataFetcher
 */
class pwp_daily_data_fetcher {

  /**
   * @var \EntityFieldQuery
   */
  protected $query;

  /**
   * DailyDataFetcher constructor.
   */
  public function __construct() {
    $this->query = new EntityFieldQuery();
  }

  /**
   * Fetchers daily game data.
   *
   * @return \stdClass
   *  The game data
   *
   * @throws \Exception
   */
  public function fetchDailyGameData() {
    $now = new \DateTime();
    $this->query
      ->entityCondition('entity_type', 'daily_show')
      ->fieldCondition('field_daily_game_date', 'value', $now->format('Y-m-d 00:00:00'));

    $result = $this->query->execute();

    $showData = new stdClass();

    if (isset($result['daily_show'])) {
      $entities = entity_load('daily_show', array_keys($result['daily_show']));
      $showEntity = array_shift($entities);
      $showEntityWrapper = entity_metadata_wrapper('daily_show', $showEntity);
      $showData->id = $showEntity->id;
      $showData->title = $showEntity->title;
      $showData->company = $showEntityWrapper->field_company->value();
      $showData->venue = $showEntityWrapper->field_venue->value();
      $showData->type = $showEntityWrapper->field_show_type->value();
      $showData->matches = [];

      /** @var \Entity $match */
      foreach ($showEntityWrapper->field_daily_matches->value() as $match) {
        $matchData = new stdClass();

        $showData->matches[] = $matchData;
        $matchData->id = $match->id;
        $matchData->title = $match->title;
        $matchData->teams = [];

        foreach (pwp_daily_constants::TEAM_KEYS as $teamKey) {
          $teamFieldName = pwp_daily_helpers::getTeamFieldName($teamKey);
          $wrestlers = $match->wrapper()->{$teamFieldName}->value();

          if (empty($wrestlers)) {
            continue;
          }

          try {
            $matchData->teams[$teamKey] = [];

            foreach ($wrestlers as $wrestler) {
              $wrestlerData = new stdClass();
              $wrestlerData->id = $wrestler->id;
              $wrestlerData->title = $wrestler->title;
              $matchData->teams[$teamKey][] = $wrestlerData;
            }
          } catch (\Exception $exception) {
            watchdog('pwp_daily', $exception->getMessage());
          }
        }
      }
    }

    return $showData;
  }

}
