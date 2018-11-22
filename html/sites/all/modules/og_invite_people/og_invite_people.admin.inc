<?php

/**
 * Settings form callback.
 */
function og_invite_people_settings_form($form, &$form_state) {
  $form['og_invite_people_roles_as_checkboxes'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use checkboxes for role selection'),
    '#description' => t('On the user invite form, use checkboxes instead of a dropdown to select roles.'),
    '#default_value' => variable_get('og_invite_people_roles_as_checkboxes', 0),
  );

  $form['og_invite_people_add_membership_state'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include the membership state'),
    '#description' => t('On the user invite form, include the "Membership state" dropdown, or simply set membership state active.'),
    '#default_value' => variable_get('og_invite_people_add_membership_state', 0),
  );

  $form['og_invite_people_single'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use single user invite form'),
    '#description' => t('Invite a single user and maybe also set user fields.'),
    '#default_value' => variable_get('og_invite_people_single', 0),
  );

  $form['og_invite_people_attached_user_fields'] = array(
    '#type' => 'checkboxes',
    '#title' => t('User fields'),
    '#description' => t('Select fields to include in the invite form. This usually only makes sense on single user invite form.'),
    '#default_value' => variable_get('og_invite_people_attached_user_fields', array()),
    '#options' => og_invite_people_user_field_options(),
    '#multiple' => TRUE,
    '#weight' => 95,
  );

  $form['og_invite_people_use_fieldset'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use fieldset'),
    '#description' => t('Display the invite form in a fieldset.'),
    '#default_value' => variable_get('og_invite_people_use_fieldset', 1),
  );

  $form['og_invite_people_add_invite_tab'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add invite tab'),
    '#description' => t('Add an invite tab on the group entity page.'),
    '#default_value' => variable_get('og_invite_people_add_invite_tab', 0),
  );

  $form['og_invite_people_remove_og_tab'] = array(
    '#type' => 'checkbox',
    '#title' => t('Remove OG admin tab'),
    '#description' => t('Remove the group admin tab on the group entity page.'),
    '#default_value' => variable_get('og_invite_people_remove_og_tab', 0),
  );

  $form['og_invite_people_additional_settings_title'] = array(
    '#type' => 'item',
    '#title' => t('Email invite templates'),
    '#description' => t('Email templates for invited Users.'),
    '#weight' => 96
  );

  $form['og_invite_people_additional_settings'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 97,
  );

  $form['og_invite_people_email_existing'] = array(
    '#type' => 'fieldset',
    '#title' => t('Existing Users'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    // The #group value must match the name of the vertical tabs element.
    // In most cases, this is 'additional_settings'.
    '#group' => 'og_invite_people_additional_settings',
    //'#tree' => TRUE,
    '#weight' => 1,
  );
  $form['og_invite_people_email_existing']['og_invite_people_email_existing_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => variable_get('og_invite_people_email_existing_subject', 'Membership details for [user:name] at [site:name] in group [node:title]'),
  );
  $form['og_invite_people_email_existing']['og_invite_people_email_existing_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#default_value' => variable_get('og_invite_people_email_existing_body', "[user:name],

You have been added as a member to [@node:title]. You can log in and view your memberships at [site:login-url].

--  [site:name] team"),
  );

  $form['og_invite_people_email_new'] = array(
    '#type' => 'fieldset',
    '#title' => t('New Users'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    // The #group value must match the name of the vertical tabs element.
    // In most cases, this is 'additional_settings'.
    '#group' => 'og_invite_people_additional_settings',
    //'#tree' => TRUE,
    '#weight' => 2,
  );
  $form['og_invite_people_email_new']['og_invite_people_email_new_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => variable_get('og_invite_people_email_new_subject', 'Membership details for [user:name] at [site:name]'),
  );
  $form['og_invite_people_email_new']['og_invite_people_email_new_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#default_value' => variable_get('og_invite_people_email_new_body', "[user:name],

A site administrator at [site:name] has created an account for you. You may now log in by clicking this link or copying and pasting it to your browser:

[user:one-time-login-url]

This link can only be used once to log in and will lead you to a page where you can set your password.

After setting your password, you will be able to log in at [site:login-url] in the future using:

username: [user:name]
password: Your password

You also have been added as a member to [node:title].

--  [site:name] team"),
  );

  $form['#validate'][] = 'og_invite_people_settings_form_validate';
  $form['#submit'][] = 'og_invite_people_check_menu_rebuild_needed';

  return system_settings_form($form);
}

/**
 * Settings form validate callback.
 */
function og_invite_people_settings_form_validate($form, &$form_state) {
  // Normalize.
  $fields =& $form_state['values']['og_invite_people_attached_user_fields'];
  $fields = array_filter($fields);
}

function og_invite_people_check_menu_rebuild_needed($form, &$form_state) {
  foreach (array('og_invite_people_add_invite_tab', 'og_invite_people_remove_og_tab') as $var) {
    $old_value = variable_get($var, 0);
    $new_value = $form_state['values'][$var];
    if ($old_value != $new_value) {
      variable_set('menu_rebuild_needed', TRUE);
    }
  }
}

/**
 * Callback to return user fields as form options.
 */
function og_invite_people_user_field_options() {
// Get all user's required fields.
  $user_fields = field_info_instances('user', 'user');
  $field_options = array();
  foreach ($user_fields as $field_name => $field_info) {
    $field_option = $field_info['label'];
    if (!empty($field_info['required'])) {
      $field_option .= '*';
    }
    if (!empty($field_info['settings']['user_register_form'])) {
      $field_option .= '*';
    }
    $field_options[$field_name] = $field_option;
  }
  return $field_options;
}
