<?php
/**
 * @file og_invite_people.inc
 */


$plugin = array(
  'content types' => 'og_invite_people_content_types',
  'render callback' => 'og_invite_people_pane_render',
  'edit form' => 'og_invite_people_pane_config',
  'category' => array(t('Organic groups'), -9),
  'admin title' => t('OG invite people'),
);

/**
 * Return all content types available.
 */
function og_invite_people_content_types($plugin) {
  $types = &drupal_static(__FUNCTION__, NULL);
  if (isset($types)) {
    return $types;
  }

  $types = array();
  foreach (og_get_all_group_bundle() as $entity_type => $bundles) {
    $entity_info = entity_get_info($entity_type);
    $entity_label = $entity_info['label'];
    $types[$entity_type] = array(
      'title' => t('OG invite people (@entity_type)', array('@entity_type' => $entity_label)),
      'required context' => new ctools_context_required($entity_label, $entity_type, array(
       'type' => array_keys($bundles),
      )),
    ) + $plugin;
  }
  return $types;
}

function og_invite_people_pane_render($entity_type, $conf, $panel_args, $context) {
  if (empty($context) || empty($context->data)) {
    return;
  }

  // Get the entity.
  $entity = $context->data;
  list($entity_id, , ) = entity_extract_ids($entity_type, $entity);

  // Include form function.
  module_load_include('inc', 'og_invite_people', 'og_invite_people.form');

  // Build the block.
  $block = new stdClass();
  $block->module  = 'og_invite_people';
  if (!empty($conf['override_title'])) {
    $block->title = filter_xss_admin($conf['override_title_text']);
  }
  else {
    $block->title = t('Invite People');
  }
  $block->content = drupal_get_form('_og_invite_people_new_users_form', $entity_type, $entity_id);
  $block->delta   = $entity_id;

  return $block;
}

function og_invite_people_pane_config($form, &$form_state) {
  return $form;
}