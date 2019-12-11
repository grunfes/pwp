<?php

/**
 * wp_daily_data_save menu callback
 */
function pwp_daily_data_save() {
  try {
    $requestMethod = strtoupper($_SERVER['REQUEST_METHOD']);

    if ($requestMethod === 'POST') {
      $requestData = file_get_contents("php://input", TRUE);

      if (($data = json_decode($requestData, TRUE)) !== NULL && !empty($data['show']) && isset($data['picks']) && is_array($data['picks'])) {
        $dataSaver = new pwp_daily_data_saver();
        $dataSaver->save($data);

        drupal_json_output([
          'status' => 200,
          'data' => '',
        ]);
        exit();
      }

      throw new \Exception('Invalid Request Data.');
    }

    throw new \Exception("Request Method({$requestMethod}) not supported.");
  }
  catch (\Exception $exception) {
    watchdog('pwp_daily', $exception->getMessage(), [], WATCHDOG_CRITICAL);
    drupal_add_http_header('Status', '500 Internal Server Error');
  }
}

