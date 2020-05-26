<?php

/**
 * Class DailyDataFetcher
 */
class pwp_daily_data_fetcher {

  /**
   * @return null|array
   * @throws \Exception
   */
  public function fetchUserDailyGameSummary() {
    global $user;
    $dailyShowEntity = $this->getDailyShowEntity();

    if (!$dailyShowEntity) {
      return NULL;
    }

    $query = new EntityFieldQuery();
    $query
      ->entityCondition('entity_type', 'daily_results')
      ->fieldCondition('field_daily_user_reference', 'target_id', $user->uid)
      ->fieldCondition('field_daily_show_reference', 'target_id', $dailyShowEntity->id);

    $result = $query->execute();

    if (isset($result['daily_results'])) {
      $entityIds = array_keys($result['daily_results']);
      return entity_load('daily_results', $entityIds);
    }

    return NULL;
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
    $showEntity = $this->getDailyShowEntity();
    $showData = new stdClass();

    $kk = $this->fetchUserDailyGameSummary();

    if ($showEntity) {
      $showEntityWrapper = entity_metadata_wrapper('daily_show', $showEntity);
      $showData->id = $showEntity->id;
      $showData->title = $showEntity->title;
      $showData->company = $showEntityWrapper->field_company->value();
      $showData->venue = $showEntityWrapper->field_venue->value();
      $showData->type = $showEntityWrapper->field_show_type->value();
      $showData->perfect_ppv_bonus = intval($showEntityWrapper->field_perfect_ppv_bonus->value());
      $showData->matches = [];

      /** @var \Entity $match */
      foreach ($showEntityWrapper->field_daily_matches->value() as $match) {
        if (!isset($match->field_daily_teams['und']) || !is_array($match->field_daily_teams['und']) || count($match->field_daily_teams['und']) === 0) {
          throw new \Exception('No teams assigned to ' . $match->id);
        }

        $matchData = new stdClass();
        $showData->matches[] = $matchData;
        $matchData->id = $match->id;
        $matchData->title = $match->title;
        $matchData->teams = [];
        $matchData->points = intval($match->wrapper()->field_point_to_earn->value());

        $dailyTeamIds = array_column($match->field_daily_teams['und'], 'target_id');
        foreach (entity_load('daily_team', $dailyTeamIds) as $dailyTeam) {
          if (!isset($dailyTeam->field_daily_wrestlers['und']) || !is_array($dailyTeam->field_daily_wrestlers['und']) || count($dailyTeam->field_daily_wrestlers['und']) === 0) {
            throw new \Exception('No wrestlers assigned to team: ' . $dailyTeam->id);
          }

          $dailyTeamData = new stdClass();
          $dailyTeamData->id = $dailyTeam->id;
          $dailyTeamData->title = $dailyTeam->title;
          $dailyTeamData->wrestlers = [];

          $dailyTeamWrestlerIds = array_column($dailyTeam->field_daily_wrestlers['und'], 'target_id');
          foreach (entity_load('daily_wrestler', $dailyTeamWrestlerIds) as $dailyWrestler) {
            $dailyWrestlerData = new stdClass();
            $dailyWrestlerData->id = $dailyWrestler->id;
            $dailyWrestlerData->title = $dailyWrestler->title;
            $dailyTeamData->wrestlers[] = $dailyWrestlerData;
          }

          $matchData->teams[] = $dailyTeamData;
        }
      }
    }

    return $showData;
  }

  /**
   * @return mixed|null
   * @throws \Exception
   */
  public function getDailyShowEntity() {
    $now = new \DateTime();
    $query = new EntityFieldQuery();
    $query
      ->entityCondition('entity_type', 'daily_show')
      ->fieldCondition('field_daily_game_date', 'value', $now->format('Y-m-d 00:00:00'));

    $result = $query->execute();

    if (isset($result['daily_show'])) {
      $showEntities = entity_load('daily_show', array_keys($result['daily_show']));
      return array_shift($showEntities);
    }

    return NULL;
  }

}
