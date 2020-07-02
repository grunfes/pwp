<?php

/**
 * Class DailyDataFetcher
 */
class pwp_daily_data_fetcher {

  /**
   * @return \stdClass|NULL
   * @throws \Exception
   */
  public function fetchUserDailyGameSummary($showEntity) {
    global $user;
    $data = NULL;

    if (!$showEntity) {
      return $data;
    }

    $query = new EntityFieldQuery();
    $query
      ->entityCondition('entity_type', 'daily_results')
      ->fieldCondition('field_daily_user_reference', 'target_id', $user->uid)
      ->fieldCondition('field_daily_show_reference', 'target_id', $showEntity->id);

    $result = $query->execute();

    if (isset($result['daily_results']) && count($result['daily_results']) > 0) {
      $data = new stdClass();
      $entityIds = array_keys($result['daily_results']);
      $entities = entity_load('daily_results', $entityIds);
      $entity = array_pop($entities);
      $entityWrapper = entity_metadata_wrapper('daily_results', $entity);

      $data->perfectGame = $entityWrapper->field_daily_perfect_game->value();
      $data->score = $entityWrapper->field_daily_score->value();
      $data->bonus = floatval($showEntity->wrapper()->field_perfect_ppv_bonus->value());

      $data->picks = array_map(function ($pick) {
        $pickData = new stdClass();
        $pickWrapper = $pick->wrapper();
        $pickMatch = $pickWrapper->field_daily_pick_match->value()->wrapper();

        $pickData->match = $pickMatch->title->value();
        $pickData->points = floatval($pickMatch->field_point_to_earn->value());
        $pickData->teams = [];

        if (boolval($pickWrapper->field_daily_no_contest->value())) {
          $pickData->notContest = TRUE;
        }
        else {
          $userPickWrapper = $pickWrapper->field_daily_pick_team->value()->wrapper();
          $pickData->teams = array_map(function ($team) use ($userPickWrapper) {
            $teamData = new stdClass();
            $teamWrapper = $team->wrapper();
            $teamData->title = $teamWrapper->title->value();
            $teamData->userPick = $userPickWrapper->id->value() == $teamWrapper->id->value();
            $teamData->winningTeam = boolval($teamWrapper->field_daily_winning_team->value());
            $teamData->wrestlers = array_map(function ($wrestler) {
              return $wrestler->title;
            }, $teamWrapper->field_daily_wrestlers->value());

            return $teamData;
          }, $pickMatch->field_daily_teams->value());
        }

        return $pickData;
      }, $entityWrapper->field_daily_picks_reference->value());
    }

    return $data;
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
    global $user;

    $showEntity = $this->getDailyShowEntity();
    $data = new stdClass();
    $data->hasUserPlayed = false;
    $data->hasDailyGame = $showEntity instanceof Entity;
    $data->show = new stdClass();
    $data->user = new stdClass();
    $data->user->name = $user->name;
    $data->summary = new stdClass();

    $userDailyGameSummary =  $this->fetchUserDailyGameSummary($showEntity);
    if ($userDailyGameSummary !== NULL) {
      $data->hasUserPlayed = true;
      $data->summary = $userDailyGameSummary;
    }

    if ($showEntity) {
      $showEntityWrapper = entity_metadata_wrapper('daily_show', $showEntity);
      $data->show->id = $showEntity->id;
      $data->show->title = $showEntity->title;
      $data->show->company = $showEntityWrapper->field_company->value();
      $data->show->venue = $showEntityWrapper->field_venue->value();
      $data->show->type = $showEntityWrapper->field_show_type->value();
      $data->show->perfect_ppv_bonus = intval($showEntityWrapper->field_perfect_ppv_bonus->value());
      $data->show->matches = [];

      /** @var \Entity $match */
      foreach ($showEntityWrapper->field_daily_matches->value() as $match) {
        if (!isset($match->field_daily_teams['und']) || !is_array($match->field_daily_teams['und']) || count($match->field_daily_teams['und']) === 0) {
          throw new \Exception('No teams assigned to ' . $match->id);
        }

        $matchData = new stdClass();
        $data->show->matches[] = $matchData;
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

    return $data;
  }

  /**
   * @return mixed|null
   * @throws \Exception
   */
  public function getDailyShowEntity() {
    try {
      $dailyGameId = variable_get(\pwp_daily_constants::DAILY_GAME_VARIABLE);
      $entities = [];

      if (is_int($dailyGameId)) {
        $entities = entity_load('daily_show', [$dailyGameId]);
      }

      if (isset($entities)) {
        return array_shift($entities);
      }
    }
    catch (\Exception $exception) {
      watchdog('pwp_daily', $exception->getMessage(), NULL, WATCHDOG_WARNING);
    }

    return NULL;
  }

}
