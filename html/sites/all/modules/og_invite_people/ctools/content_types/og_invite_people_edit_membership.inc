<?php
/**
 * @file og_invite_people_edit_membership.inc
 */

if (module_exists('og_ui')) {
  $plugin = array(
    'content types' => 'og_invite_people_edit_membership_content_types',
    'render callback' => 'og_invite_people_edit_membership_render',
    'edit form' => 'og_invite_people_edit_membership_config',
    'category' => array(t('Organic groups'), -9),
    'admin title' => t('Edit og membership (by OG invite people)'),
  );
}

/**
 * Return all content types available.
 */
function og_invite_people_edit_membership_content_types($plugin) {
  $types = &drupal_static(__FUNCTION__, NULL);
  if (isset($types)) {
    return $types;
  }

  $types = array();
  foreach (og_get_all_group_bundle() as $entity_type => $bundles) {
    $entity_info = entity_get_info($entity_type);
    $entity_label = $entity_info['label'];
    $types[$entity_type] = array(
      'title' => t('Edit og membership (@entity_type)', array('@entity_type' => $entity_label)),
      'required context' => array(
          new ctools_context_required($entity_label, $entity_type, array('type' => array_keys($bundles))),
          new ctools_context_required(t('OG membership'), 'og_membership'),
      ),
    ) + $plugin;
  }
  return $types;
}

function og_invite_people_edit_membership_render($entity_type, $conf, $panel_args, $context) {
  if (empty($context) || empty($context[0]->data) || empty($context[1]->data)) {
    return;
  }

  // Get the entity.
  $entity = $context[0]->data;
  list($entity_id, , ) = entity_extract_ids($entity_type, $entity);
  $entity_wrapper = entity_metadata_wrapper($entity_type, $entity);
  // Get the membership.
  $membership = $context[1]->data;
  list($membership_id, , ) = entity_extract_ids('og_membership', $membership);

  // Include form function.
  module_load_include('inc', 'og_ui', 'og_ui.admin');

  // Build the block.
  $block = new stdClass();
  $block->module  = 'og_invite_people';
  if (!empty($conf['override_title'])) {
    $block->title = filter_xss_admin($conf['override_title_text']);
  }
  else {
    $block->title = t('Edit membership in group @group', array('@group' => $entity_wrapper->label()));
  }
  $block->content = drupal_get_form('og_ui_edit_membership', $entity_type, $entity_id, $membership);
  $block->delta   = "$entity_type:$entity_id:$membership_id";

  return $block;
}

function og_invite_people_edit_membership_config($form, &$form_state) {
  return $form;
}