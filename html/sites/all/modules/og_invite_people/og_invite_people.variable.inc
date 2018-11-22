<?php
/**
 * @file og_invite_people.variable.inc
 */

/**
 * Default callback for user mail variables
 */
function og_invite_people_variable_mail_default($variable, $options) {
  // Remove 'og_invite_people_mail_' prefix
  $name = substr($variable['name'], 22);
  // We need the last $use_i18n=FALSE parameter to prevent recursion.
  return _og_invite_people_email_text($name, $options['language'], array(), TRUE, FALSE);
}

/**
 * Implements hook_variable_info().
 */
function og_invite_people_variable_info($options) {
  $email_token_help = ' ' . t('Available variables are: [site:name], [site:url], [user:name], [user:mail], [site:login-url], [site:url-brief], [user:edit-url], [user:one-time-login-url], [user:cancel-url].', array(), $options);
  $email_token_help .= ' ' . t('Also you can use [node:*] tokens for node groups, or likewise for other entity groups.', array(), $options);

  $defaults = array(
    'type' => 'user_mail',
    'group' => 'user_mails',
    'repeat' => array(
      'default callback' => 'og_invite_people_variable_mail_default',
    ),
  );

  $variables['og_invite_people_mail_existing_user_new_group_[mail_part]'] =
    $defaults + array(
      'title' => t('Invitation of existing user to group confirmation'),
      'description' => t('Edit the e-mail messages sent to users when they are invited to a group and they already have an account.', array(), $options) . $email_token_help,
    );
  $variables['og_invite_people_mail_new_user_new_group_[mail_part]'] =
    $defaults + array(
      'title' => t('Invitation of new user to group confirmation'),
      'description' => t('Edit the e-mail messages sent to users when they are invited to a group and an account is created for them.', array(), $options) . $email_token_help,
    );
  return $variables;
}
