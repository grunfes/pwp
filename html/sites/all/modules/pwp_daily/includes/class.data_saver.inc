<?php

/**
 * Class pwp_daily_data_saver
 */
class pwp_daily_data_saver {

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
   * @param array $data
   *
   * @throws \Exception
   */
  public function save($data) {
    global $user;

    $transaction = db_transaction();
    $dataFetcher = new pwp_daily_data_fetcher();

    try {
      $showEntity = $dataFetcher->getDailyShowEntity();
      $userDailyGameSummary = $dataFetcher->fetchUserDailyGameSummary($showEntity);

      if ($userDailyGameSummary !== NULL) {
        return $userDailyGameSummary;
      }

      if ($showEntity instanceof Entity && $showEntity->wrapper()
          ->get('id')
          ->value() === $data['show']
      ) {
        $matches = $showEntity->wrapper()->field_daily_matches->value();
        $validKeys = [];

        foreach ($matches as $matchEntity) {
          if (!($matchEntity instanceof Entity)) {
            throw new \Exception('Not a Valid Entity.');
          }

          $validKeys[$matchEntity->id] = [];
          foreach ($matchEntity->wrapper()->field_daily_teams->value() as $team) {
            $validKeys[$matchEntity->id][] = $team->id;
          }
          $validKeys[$matchEntity->id][] = '-1';
        }

        $savedPicks = array();

        if (!empty($data['picks'])) {
          foreach ($data['picks'] as $pick) {
            $this->validatePick($pick, $validKeys);
            $savedPicks[] = $this->savePick(array_merge(
              $pick,
              [
                'show' => $data['show'],
                'user' => $user->uid,
              ]
            ));
          }
        }

        $this->saveResult($showEntity, $savedPicks, $user);

        return $dataFetcher->fetchUserDailyGameSummary($showEntity);
      }
    }
    catch (\Exception $exception) {
      $transaction->rollback();
    }

    throw new \Exception("No Daily Show Found({$data['show']}).");
  }

  /**
   * @param \Entity $show
   * @param \Entity[] $picks
   * @param \stdClass $user
   *
   * @throws \EntityMetadataWrapperException
   */
  protected function saveResult($show, $picks, $user) {
    $title = sprintf(
      '%s %s results (%s)',
      $show->title, $user->name,
      $show->field_daily_game_date['und'][0]['value']
    );

    $dailyResult = entity_create('daily_results', [
      'type' => 'daily_results',
      'title' => $title,
    ]);

    $dailyResultWrapper = entity_metadata_wrapper('daily_results', $dailyResult);
    $dailyResultWrapper->field_daily_show_reference = $show;
    $dailyResultWrapper->field_daily_picks_reference = $picks;
    $dailyResultWrapper->field_daily_user_reference = $user;

    $score = $this->calculateScore($picks, $show);
    $dailyResultWrapper->field_daily_score = $score->points;
    $dailyResultWrapper->field_daily_perfect_game = $score->perfectGame;

    $dailyResultWrapper->save();
  }

  /**
   * @param $pickData
   * @param $user
   *
   * @throws \Exception
   */
  protected function savePick($pickData) {
    $dailyPick = entity_create('daily_pick', [
      'type' => 'daily_pick',
      'title' => 'Daily Pick',
    ]);

    if (!($dailyPick instanceof Entity)) {
      throw new \Exception('Unable to create User Pick');
    }

    $dailyPickWrapper = entity_metadata_wrapper('daily_pick', $dailyPick);
    $dailyPickWrapper->field_daily_pick_user = $pickData['user'];
    $dailyPickWrapper->field_daily_pick_show = $pickData['show'];

    if (isset($pickData['match'])) {
      $dailyPickWrapper->field_daily_pick_match = $pickData['match'];
    }

    if (isset($pickData['team'])) {
      if ($pickData['team'] === pwp_daily_constants::DAILY_NO_CONTEST_KEY) {
        $dailyPickWrapper->field_daily_no_contest->set(\pwp_daily_constants::DAILY_NO_CONTEST_TRUE);
      }
      else {
        $dailyPickWrapper->field_daily_pick_team = $pickData['team'];
      }
    }
    $dailyPickWrapper->save();
    return $dailyPick;
  }

  /**
   * @param array $pick
   * @param array $validKeys
   *
   * @throws \Exception
   */
  protected function validatePick($pick, $validKeys) {
    if (!is_array($pick) || empty($pick['match']) || empty($pick['team'])) {
      $serialized = serialize($pick);
      throw new \Exception("Missing Pick Data({$serialized}).");
    }

    if (!array_key_exists($pick['match'], $validKeys) || !in_array($pick['team'], $validKeys[$pick['match']])) {
      throw new \Exception('Invalid Pick Key.');
    }
  }

  /**
   * @param \Entity[] $picks
   * @param \Entity $show
   *
   * @return \stdClass
   */
  protected function calculateScore($picks, $show) {
    $score = new stdClass();
    $score->perfectGame = false;
    $winningMatchesCount = 0;

    $score->points = array_reduce($picks, function ($points, $pick) use (&$winningMatchesCount, $show) {
      $pickWrapper = $pick->wrapper();
      $matchWrapper = $pickWrapper->field_daily_pick_match->value()->wrapper();
      $isNoContestMatch = array_reduce($matchWrapper->field_daily_teams->value(), function ($noContest, $team) {
          $winningTeam = boolval($team->wrapper()->field_daily_winning_team->value());
          return $noContest && !$winningTeam;
      }, TRUE);

      if (
        ($isNoContestMatch && boolval($pickWrapper->field_daily_no_contest->value())) ||
        ($pickWrapper->field_daily_pick_team->value() && $pickWrapper->field_daily_pick_team->value()->wrapper()->field_daily_winning_team->value())
      ) {
        $winningMatchesCount += 1;
        return $points + floatval($pickWrapper->field_daily_pick_match->value()->wrapper()->field_point_to_earn->value());
      }
      return $points;
    }, 0.0);

    if (count($show->wrapper()->field_daily_matches->value()) === $winningMatchesCount) {
      $score->perfectGame = true;
      $score->points += floatval($show->wrapper()->field_perfect_ppv_bonus->value());
    }

    return $score;
  }
}
