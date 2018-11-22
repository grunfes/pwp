<?php

/**
 * Main OG Invite People form.
 */
function _og_invite_people_new_users_form($form, &$form_state, $group_type, $gid) {
  og_set_breadcrumb($group_type, $gid, array(l(t('Group'), "$group_type/$gid/group")));
  $group = entity_load_single($group_type, $gid);
  $label = entity_label($group_type, $group);
  list(,, $bundle) = entity_extract_ids($group_type, $group);
  // For translation parameter see https://www.drupal.org/node/1473262
  $og_roles = og_roles($group_type, $bundle, $gid, FALSE, FALSE, TRUE);
  $single_invite_form = variable_get('og_invite_people_single', 0);

  $form['group_type'] = array('#type' => 'value', '#value' => $group_type);
  $form['gid'] = array('#type' => 'value', '#value' => $gid);

  if (variable_get('og_invite_people_use_fieldset', 1)) {
    $form['og_invite_people'] = array(
      '#type' => 'fieldset',
      '#title' => $single_invite_form ?
        t('Invite a new user to %group', array('%group' => $label)) :
        t('Invite new users to %group', array('%group' => $label)),
    );
  }
  else {
    $form['og_invite_people'] = array(
      '#type' => 'container',
    );
  }

  // Set #parents to 'top-level' if it doesn't exist.
  $form += array('#parents' => array());
  // If no language is provided use the default site language.
  $options = array(
    'language' => LANGUAGE_NONE,
    'default' => TRUE,
  );

  $attached_user_fields = variable_get('og_invite_people_attached_user_fields', array());

  ctools_include('fields');
  foreach ($attached_user_fields as $field_name) {
    $field_instance = field_info_instance('user', $field_name, 'user');
    $form['og_invite_people'] += (array) ctools_field_invoke_field($field_instance, 'form', 'user', NULL, $form, $form_state, $options);
  }

  // Pass info about required field to form_state values.
  $form['og_invite_people']['attached_fields'] = array(
    '#type' => 'value',
    '#value' => $attached_user_fields,
  );

  if ($single_invite_form) {
    $form['og_invite_people']['invitee'] = array(
      '#type' => 'textfield',
      '#title' => t('Email'),
      '#description' => t('New user will be created and invited as member of this group. A one time login link will be sent to the invitee\'s email.  '),
      '#required' => TRUE,
    );
  }
  else {
    $form['og_invite_people']['invitee'] = array(
      '#type' => 'textarea',
      '#title' => t('User email(s)'),
      '#description' => t('Use commas or new lines to split email addresses. The new users will be created and invited as members of this group. A one time login link will be sent to the invitees\' email.  '),
      '#required' => TRUE,
    );
  }
  $role_selection_widget = variable_get('og_invite_people_roles_as_checkboxes', 0) ? 'checkboxes' : 'select';
  $form['og_invite_people']['roles'] = array(
    '#type' => $role_selection_widget,
    '#multiple' => TRUE,
    '#title' => t('Assign roles'),
    '#description' => t('Choose one or more roles you would like to assign to the users. ("member" role is assigned by default)'),
    '#options' => $og_roles,
    '#size' => 5,
    '#weight' => 1001,
  );
  if (variable_get('og_invite_people_add_membership_state', 0)) {
    $form['og_invite_people']['state'] = array(
      '#type' => 'select',
      '#title' => t('Membership state'),
      '#options' => og_group_content_states(),
      '#description' => t('Select the state of the membership.'),
      '#weight' => 1002,
    );
  }
  else {
    $form['og_invite_people']['state'] = array(
      '#type' => 'value',
      '#value' => OG_STATE_ACTIVE,
    );

  }

  $og_field_names = og_get_group_audience_fields('user', 'user', $group_type);
  $og_field_name = !empty($form_state['values']['field_name']) ? $form_state['values']['field_name'] : key($og_field_names);

  if (count($og_field_names) > 1) {
    $form['og_invite_people']['og_field_name'] = array(
      '#type' => 'select',
      '#title' => t('Field name'),
      '#options' => $og_field_names,
      '#default_value' => $og_field_name,
      '#description' => t('Select the field name, the group membership should be registered in.'),
      '#weight' => 1003,
    );
  }
  else {
    // There is only a single field name, so just pass it as a value.
    $form['og_invite_people']['og_field_name'] = array(
      '#type' => 'value',
      '#value' => $og_field_name,
    );
  }

  $form_state['og_field_name'] = $og_field_name;

  $form['actions'] = array('#type' => 'actions');
  $submit_text = $single_invite_form ? t('Invite user') : t('Invite user(s)');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => $submit_text);

  return $form;
}


/**
 * Validation handler for _og_invite_people_new_users_form.
 */
function _og_invite_people_new_users_form_validate($form, &$form_state) {
  $invitee_emails = $form_state['values']['invitee'];
  $single_invite_form = variable_get('og_invite_people_single', 0);
  if ($single_invite_form) {
    $invitee_emails = array($invitee_emails);
  }
  else {
    $invitee_emails = preg_split('/\s*[,\n]\s*/', $invitee_emails);
  }
  $invitee_emails = array_map('trim', $invitee_emails);
  $invitee_emails = array_filter($invitee_emails);

  $form_state['invitee_emails'] = array();
  foreach ($invitee_emails as $invitee_email) {
    if (!valid_email_address($invitee_email)) {
      form_set_error('invitee', t('Invalid email ' . $invitee_email));
    }
    else {
      $form_state['invitee_emails'][] = $invitee_email;
    }
  }
}

/**
 * Submit handler for _og_invite_people_new_users_form.
 */
function _og_invite_people_new_users_form_submit($form, &$form_state) {
  $group_type = $form_state['values']['group_type'];
  $gid = $form_state['values']['gid'];
  $invitee_emails = $form_state['invitee_emails'];
  $og_roles = $form_state['values']['roles'];
  $state = $form_state['values']['state'];
  $og_field_name = $form_state['og_field_name'];
  $entity_type = 'user';
  $all_states = og_group_content_states();

  // Add group membership form.
  $values = array();

  foreach ($invitee_emails as $invitee_email) {
    $had_account = FALSE;

    // Try to load the user by email and see if exists.
    if ($account = user_load_by_mail($invitee_email)) {
      $had_account = TRUE;
      // $message = '%user is already known and has been added to the group.';
    }
    else {
      // Create a user and return $account object.
      $account = _og_invite_people_create_user($invitee_email, $form_state['values']);
    }

    // Check if user is already assigned to the group. Any "state" (active, pending,... counts as existing).
    if ($had_account && og_is_member($group_type, $gid, $entity_type, $account, array())) {
      // Load OG membership.
      $og_membership = og_get_membership($group_type, $gid, $entity_type, $account->uid);
      $message = t('%user is already a member of this group.', array('%user' => format_username($account)));
    }
    elseif ($had_account) {
      // Add existing user to group.
      $og_membership = og_membership_create($group_type, $gid, $entity_type, $account->uid, $og_field_name, $values);
      $message = t('%user has been added to the group and an email notification was sent.', array('%user' => format_username($account)));
      // Send the e-mail
      drupal_mail('og_invite_people', 'existing_user_new_group', $invitee_email, $GLOBALS['language'], array('account' => $account, 'gid' => $gid, 'group_type' => $group_type), variable_get('site_mail', NULL));
    }
    else {
      // Create OG membership and add user to group.
      $og_membership = og_membership_create($group_type, $gid, $entity_type, $account->uid, $og_field_name, $values);
      $message = t('%user has been created and an email notification was sent.', array('%user' => format_username($account)));
      // Send the e-mail
      drupal_mail('og_invite_people', 'new_user_new_group', $invitee_email, $GLOBALS['language'], array('account' => $account, 'gid' => $gid, 'group_type' => $group_type), variable_get('site_mail', NULL));
    }

    // Update OG memebership if necessary.
    if ($og_membership->state != $state) {
      $message .= t(' Membership state changed from "%prev_state" to "%post_state"', array('%prev_state' => $all_states[$og_membership->state], '%post_state' => $all_states[$state]));
      $og_membership->state = $state;
    }

    // Save the OG membership.
    $og_membership->save();

    // Add OG roles to users.
    foreach ($og_roles as $rid) {
      og_role_grant($group_type, $gid, $account->uid, $rid);
    }

    // Offer invitee user account object in form_state.
    $form_state['account'][] = $account;

    // Invoke hook_og_invite_people_invited() - after a user is created and added to group.
    module_invoke_all('og_invite_people_invited', $account, $og_membership);

    // Print the message for each user.
    drupal_set_message($message);
  }
}

/**
 * Create new user and send email.
 *
 * @param string $invitee_email
 * @param array $values
 * @return object $account
 */
function _og_invite_people_create_user($invitee_email, $values) {
  // This will generate a random password.
  $password = user_password(8);

  // Username from email.
  $name = explode('@', $invitee_email);

  // To prepare the email address as a username, trim any potential leading
  // and trailing spaces and replace simple illegal characters with hyphens.
  // @see user_validate_name().
  $name = preg_replace('/[^\x{80}-\x{F7} a-z0-9@_.\'-]/i', '-', trim($name[0]));

  // Check if user name is available and append _NUM if existing.
  if (user_load_by_name($name)) {
    $i = 1;
    $new_name = $name . '_' . $i;
    while (user_load_by_name($new_name)) {
      $i++;
      $new_name = $name . '_' . $i;
    }
    $name = $new_name;
  }

  // Set up the user fields.
  $fields = array(
    'name' => $name,
    'mail' => $invitee_email,
    'pass' => $password,
    'status' => 1,
    'init' => 'email address',
    'roles' => array(
      DRUPAL_AUTHENTICATED_RID => 'authenticated user',
    ),
  );

  // Add required fields to $fields array.
  foreach (array_filter($values['attached_fields']) as $field_name) {
    $fields += array($field_name => $values[$field_name]);
  }

  // Create new user function.
  $account = user_save('', $fields);

  return $account;
}
