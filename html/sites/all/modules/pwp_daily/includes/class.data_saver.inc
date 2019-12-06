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

    $dataFetcher = new pwp_daily_data_fetcher();
    $showEntity = $dataFetcher->getDailyShowEntity();

    if ($showEntity instanceof Entity && $showEntity->wrapper()
        ->get('id')
        ->value() === $data['show']) {
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
      }

      foreach ($data['picks'] as $pick) {
        $this->validatePick($pick, $validKeys);
        $this->savePick(array_merge(
          $pick,
          [
            'show' => $data['show'],
            'user' => $user->uid,
          ]
        ));

        return;
      }
    }

    throw new \Exception("No Daily Show Found({$data['show']}).");
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
      'title' => pwp_daily_helpers::dailyPickTitleGenerator($pickData),
    ]);

    if (!($dailyPick instanceof Entity)) {
      throw new \Exception('Unable to create User Pick');
    }

    $dailyPickWrapper = entity_metadata_wrapper('daily_pick', $dailyPick);
    $dailyPickWrapper->field_daily_pick_user = $pickData['user'];
    $dailyPickWrapper->field_daily_pick_show = $pickData['show'];
    $dailyPickWrapper->field_daily_pick_match = $pickData['match'];
    $dailyPickWrapper->field_daily_pick_team = $pickData['team'];
    $dailyPickWrapper->save();
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

}
