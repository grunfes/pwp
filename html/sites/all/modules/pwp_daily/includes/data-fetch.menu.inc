<?php

/**
 * pwp_daily_data_fetch menu callback
 */
function pwp_daily_data_fetch() {
  $dataFetcher = new pwp_daily_data_fetcher();

  try {
    drupal_json_output([
      'status' => 200,
      'data' => $dataFetcher->fetchDailyGameData(),
    ]);
  } catch (\Exception $exception) {
    drupal_json_output([
      'status' => 500,
      'data' => $exception->getMessage(),
    ]);
  }
}
