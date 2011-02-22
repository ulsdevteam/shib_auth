<?php
/**
 * @file
 *  Roles manager forms.
 */

/**
 * Generate the shibboleth rule adding form
 *
 * @param $options contains the data, we want to fill the form with
 * @returns the edit form, with the fields already filled in with the elements of the options array
 */
function shib_auth_edit_form($options) {

  $form['shib_auth_new_id'] = array(
    '#title'          => t('Entry id'),
    '#type'           => 'hidden',
    '#default_value'  => $options[0],
  );

  $form['shib_auth_new_attrib'] = array(
    '#title'          => t('Shibboleth attribute name'),
    '#type'           => 'textfield',
    '#default_value'  => $options[1],
    '#require'        => TRUE,
    '#description'    => t('More properly: <b>!server</b> field name; enable DEBUG mode to list available fields. <br/>Note that it might differ from your users\' fields.', array('!server' => $_SERVER)),
  );

  $form['shib_auth_new_regexp'] = array(
    '#title'          => t('Value (regexp)'),
    '#type'           => 'textfield',
    '#default_value'  => $options[2],
    '#require'        => TRUE,
  );

  $roles = user_roles(TRUE);

  $form['shib_auth_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Roles'),
    '#default_value' => count($options[3]) > 1 || (count($options[3]) == 1 && $options[3] != "") ? $options[3] : array(),
    '#options' => $roles,
  );

  $form['sticky_markup'] = array(
    '#value' => '<b>Role type:</b>',
  );

  $form['shib_auth_new_sticky'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sticky'),
    '#default_value' => $options[5],
    '#description'    => t('Set the rule to be sticky if you want to save the role(s) permanently to the user\'s profile.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t($options[4]),
  );

  return $form;
} //function shib_auth_edit_form

/**
 * This function enables the administrator to clone an existing rule, this is useful,
 * when we want to create a rule, which is simiral to another one
 * @param id rule identifier
 */
function _shib_auth_clone_rule($id) {
  $rule = db_query("SELECT * FROM {shib_auth} WHERE id = :id", array(':id' => array($id)));
  $db_entry = $rule->fetchAssoc();
  $db_entry['id'] = NULL;
  $update = array();
  $ret = drupal_write_record('shib_auth', $db_entry, $update);
  if ($ret = SAVED_NEW) {
    drupal_set_message(t('The rule has been successfully cloned.'));
  }
  else {
    drupal_set_message(t('Unexpected error has been detected.'));
  }
  drupal_goto('admin/config/people/shib_auth/rules');
} //function _shib_auth_clone_rule

/**
 * This function lets the admin to delete an existing rule
 * @param id rule identifier
 */
function _shib_auth_delete_rule($id) {
  db_delete('shib_auth')
  ->condition('id', $id)
  ->execute();
  drupal_set_message(t('Rule <strong>#!id</strong> has been deleted.', array('!id' => $id)), 'warning');
  drupal_goto('admin/config/people/shib_auth/rules');
} //function _shib_auth_delete_rule

/**
 * This function lists all rules, and let the admin to do certain actions with them
 * @returns HTML table containing the attribute, RegExp, role and the actions, which can be done with each role
 */
function _shib_auth_list_rules() {
  $rules = db_query("SELECT * FROM {shib_auth}");
  $retval = '<table style="width: 100%;"><tr><th>Attribute</th><th>RegExp</th><th>Role(s)</th><th>Sticky</th><th>Actions</th></tr>' . "\n";
  $counter = 0;
  while ($rule = $rules->fetchAssoc()) {
    $roles = unserialize($rule['role']);
    $roles_list = '';
    foreach ($roles as $role) {
      $roles_list .= shib_auth_get_rolename($role) . ' ';
    }
    //     if (!empty($roles)) $roles_list = implode(', ', $roles);
    $rule['sticky'] == 1 ? $sticky = 'Yes' : $sticky = 'No';
    $retval .= '<tr><td>' . $rule['field'] . '</td><td>' . urldecode($rule['regexpression']) . '</td><td>' . $roles_list . '</td><td>' . $sticky  . '</td>';
    $retval .= '<td stype="text-align: right;">';
    $retval .= '<a href="' . url('admin/config/people/shib_auth/clone/' . $rule['id']) . '">' . t('Clone') . '</a> | ';
    $retval .= '<a href="' . url('admin/config/people/shib_auth/edit/' . $rule['id']) . '">' . t('Edit') . '</a> | ';
    $retval .= '<a href="' . url('admin/config/people/shib_auth/delete/' . $rule['id']) . '">' . t('Delete') . '</a>';
    $retval .= '</td></tr>' . "\n";
    $counter++;
  }
  if ($counter == 0) {
    $retval .= '<tr><td colspan="4" stype="text-align: center;">';
    $retval .= t('There is no rule in the database') . '</td></tr>' . "\n";
  }
  $retval .= '</table>';
  $retval .= '<a href="' . url('admin/config/people/shib_auth/new') . '">' . t('Add new rule') . '</a>';

  return $retval;
} //function _shib_auth_list_rules

/**
 * Validates a new rule
 * @param $form - the identifier of the form, which we have just received
 * @param $form_state - the state of the form, which we have just received, including all of the variables
 */
function shib_auth_new_rule_validate($form, &$form_state) {
  if (empty($form_state['values']['shib_auth_new_attrib'])) {
    form_set_error('shib_auth_new_attrib', t('This element must not be empty'));
  }
  if (empty($form_state['values']['shib_auth_new_regexp'])) {
    form_set_error('shib_auth_new_regexp', t('This element must not be empty'));
  }
} //function shib_auth_new_rule_validate

/**
 * Creates a new rule by calling universal create/edit form
 */
function shib_auth_new_rule() {
  return shib_auth_edit_form(array(0, '', '', '', 'Add rule', 0));
} //function shib_auth_new_rule

/**
 * Creates a new rule, containing he rule name, the server attrubite, the RegExp, and the role names by calling save rule
 */
function shib_auth_new_rule_submit($form, &$form_state) {
  shib_auth_save_rule($form_state, array());
} //function shib_auth_new_rule_submit

/**
 * Validates rule edit
 * @param $form - the identifier of the form, which we have just received
 * @param $form_state - the state of the form, which we have just received, including all of the variables
 */
function shib_auth_edit_rule_validate($form, &$form_state) {
  if (empty($form_state['values']['shib_auth_new_attrib'])) {
    form_set_error('shib_auth_new_attrib', t('This element must not be empty'));
  }
  if (empty($form_state['values']['shib_auth_new_regexp'])) {
    form_set_error('shib_auth_new_regexp', t('This element must not be empty'));
  }
} //function shib_auth_edit_rule_validate

/**
 * Edits a rule, containing he rule name, the server attrubite, the RegExp, and the role names by calling save rule
 */
function shib_auth_edit_rule_submit($form, &$form_state) {
  shib_auth_save_rule($form_state, "id");
} //function shib_auth_edit_rule_submit

/**
 * Saves a new rule into database
 * @param $update - decides if it is a new rule (NULL), or we're just modifing one ('id')
 * @param $form_state - the state of the form, which we have just received, including all of the variables
 */
function shib_auth_save_rule($form_state, $update) {
  $new_id = $form_state['values']['shib_auth_new_id'] == '0' ? NULL : (int) $form_state['values']['shib_auth_new_id'];
  // collect ther roles into an array
  $roles = array();
  if (is_array($form_state['values']['shib_auth_roles'])) {
    foreach ($form_state['values']['shib_auth_roles'] as $role_id) {
      $roles[] = $role_id;
    }
  }
  //save the new element into an array
  $new_element = array(
      'id'            => $new_id,
      'field'         => urlencode($form_state['values']['shib_auth_new_attrib']),
      'regexpression' => urlencode($form_state['values']['shib_auth_new_regexp']),
      'role'          => serialize($roles),
      'sticky'        => urlencode($form_state['values']['shib_auth_new_sticky']),
  );
  //write it in a record
  $ret = drupal_write_record('shib_auth', $new_element, $update);
  // if it wasn't an error
  if (empty($update)) {
    if ($ret = SAVED_NEW) {
      drupal_set_message(t('New rule has been stored.'));
    }
    else {
      drupal_set_message(t('Unexpected error has been detected.'));
    }
  }
  //an existing rule was updated
  else {
    if ($ret = SAVED_UPDATED) {
      drupal_set_message(t('The rule has been modified.'));
    }
    else {
      drupal_set_message(t('Unexpected error has been detected.'));
    }
  }
  //if everything was fine, print the rules with the newly added/modified one
  drupal_goto('admin/config/people/shib_auth/rules');
} //function shib_auth_save_rule

/**
 * Edits a rule by calling universal create/edit form
 */
function shib_auth_edit_rule($form, &$form_state) {
  $id = $form_state['build_info']['args'][0];
  $form = array();
  // calls the edit form, with the fields of the existing rule
  if (is_int((int) $id)) {
    $rule = db_query("SELECT * FROM {shib_auth} WHERE id = :id", array(':id' => $id));
    $db_entry = $rule->fetchAssoc();
    return shib_auth_edit_form(
      array($db_entry['id'], $db_entry['field'], urldecode($db_entry['regexpression']), unserialize($db_entry['role']), 'Apply', $db_entry['sticky'])
    );
  }
} //function shib_auth_edit_rule

/**
 * Admin users group membership validate
 *
 * @param $form data of the form
 * @param &$form_state contains all of the data of the form
 */
function shib_auth_admin_groups_validate($form, &$form_state) {

} //function shib_auth_admin_groups

/**
 * Admin users group membership submit
 *
 * @param $form data of the form
 * @param &$form_state contains all of the data of the form
 */
function shib_auth_admin_groups_submit($form, &$form_state) {
  variable_set('shib_auth_affilate_attrib', $form_state['values']['shib_auth_affilate_attrib']);
  drupal_set_message(t('Your changes are saved.'));
} //function shub_auth_groups_submit
