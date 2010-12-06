<?php
// $Id: shib_auth_forms.inc,v 1.6.2.23 2009/02/14 06:26:23 shafter Exp $

//
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
    '#description'    => t('More properly: <b>$_SERVER</b> field name; enable DEBUG mode to list available fields. <br/>Note that it might differ from your users\' fields.'),
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
    '#default_value' => count($options[3])>1||(count($options[3])==1 && $options[3] != "")?$options[3]:array(),
    '#options' => $roles,
  );

  $form['sticky_markup'] = array(
    '#value' => '<b>Role type:</b>',
  );

  $form['shib_auth_new_sticky'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sticky'),
    '#default_value' => $options[5],
    '#description'    => t('Set rule to be sticky, if you want the role(s) to be saved permanently to the users'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t($options[4]),
  );

  $form['#submit'][] = 'shib_auth_new_rule';

  return $form;
}//function shib_auth_edit_form()

/**
 * This function enables the administrator to clone an existing rule, this is useful,
 * when we want to create a rule, which is simiral to another one
 * @param id rule identifier
 */
function _shib_auth_clone_rule($id) {
  $rule = db_query("SELECT * FROM {shib_auth} WHERE id = %d", array($id));
  $db_entry = db_fetch_array($rule);
  $db_entry['id'] = NULL;
  $update = array();
  $ret = drupal_write_record('shib_auth', $db_entry, $update);
  if ($ret = SAVED_NEW) drupal_set_message('The rule has been successfulliy cloned.');
  else drupal_set_message('Unexpected error has been detected.');
  drupal_goto('admin/user/shib_auth/rules');
}//function _shib_auth_clone_rule()
/**
 * This function lets the admin to delete an existing rule
 * @param id rule identifier
 */
function _shib_auth_delete_rule($id) {
  db_query("DELETE FROM {shib_auth} WHERE id = %d", array($id));
  drupal_set_message('Rule <span style="font-weight: bold;">#'. $id .'</span> has been deleted.' , 'warning');
  drupal_goto('admin/user/shib_auth/rules');
}//function _shib_auth_delete_rule()
/**
 * This function lists all rules, and let the admin to do certain actions with them
 * @returns HTML table containing the attribute, RegExp, role and the actions, which can be done with each role
 */
function _shib_auth_list_rules() {
  $rules = db_query("SELECT * FROM {shib_auth}");
  $retval = '<table style="width: 100%;"><tr><th>Attribute</th><th>RegExp</th><th>Role(s)</th><th>Sticky</th><th>Actions</th></tr>'."\n";
  $counter = 0;
  while ($rule = db_fetch_array($rules)) {
    $roles = unserialize($rule['role']);
    $roles_list = '';
    foreach ($roles as $role) {
      $roles_list .= shib_auth_roles_get_rolename($role).' ';
    }
//     if (!empty($roles)) $roles_list = implode(', ', $roles);
    $rule['sticky'] == 1 ? $sticky = 'Yes' : $sticky = 'No';
    $retval .= '<tr><td>'. $rule['field'] .'</td><td>'. urldecode($rule['regexpression']) .'</td><td>'. $roles_list .'</td><td>'. $sticky  .'</td>';
    $retval .= '<td stype="text-align: right;">';
    $retval .= '<a href="'. url('admin/user/shib_auth/clone/'. $rule['id']) .'">'. t('Clone') .'</a> | ';
    $retval .= '<a href="'. url('admin/user/shib_auth/edit/'. $rule['id']) .'">'. t('Edit') .'</a> | ';
    $retval .= '<a href="'. url('admin/user/shib_auth/delete/'. $rule['id']) .'">'. t('Delete') .'</a>';
    $retval .= '</td></tr>'."\n";
    $counter++;
  }
  if ($counter == 0) {
    $retval .= '<tr><td colspan="4" stype="text-align: center;">';
    $retval .= t('There is no rule in the database') .'</td></tr>'."\n";
  }
  $retval .= '</table>';
  $retval .= '<a href="'. url('admin/user/shib_auth/new') .'">'. t('Add new rule') .'</a>';

  return $retval;
} // function _shib_auth_list_rules()
/**
 * Saves a new rule, containing he rule name, the server attrubite, the RegExp, and the role names
 *
 * @param $received_form - the identifier of the form, which we have just received
 * @returns an edit form, if there was a problem with the input values
 */
function shib_auth_new_rule($received_form) {
  $form = array();

  if ($received_form['post']['form_id'] == 'shib_auth_new_rule' ||
    $received_form['#parameters'][1]['post']['form_id'] == 'shib_auth_edit_rule') {

    $update = array();
    if (isset($received_form['#parameters'])) {
      $received_form = $received_form['#parameters'][1];
      $update = "id";
    }
    // if the received informations weren't empty
    if (!empty($received_form['post']['shib_auth_new_attrib']) &&
        !empty($received_form['post']['shib_auth_new_regexp'])) {

      $new_id = $received_form['post']['shib_auth_new_id'] == '0' ? NULL : (int) $received_form['post']['shib_auth_new_id'];
      // collect ther roles into an array
      $roles = array();
      if (is_array($received_form['post']['shib_auth_roles'])) {
        foreach ($received_form['post']['shib_auth_roles'] as $role_id) {
          $roles[] = $role_id;
        }
      }
      //save the new element into an array
      $new_element = array(
        'id'            => $new_id,
        'field'         => urlencode($received_form['post']['shib_auth_new_attrib']),
        'regexpression' => urlencode($received_form['post']['shib_auth_new_regexp']),
        'role'          => serialize($roles),
        'sticky'        => urlencode($received_form['post']['shib_auth_new_sticky']),
      );
      //write it in a record
      $ret = drupal_write_record('shib_auth', $new_element, $update);
      // if it wasn't an error
      if (empty($update)) {
        if ($ret = SAVED_NEW) drupal_set_message('New rule has been stored.');
        else drupal_set_message('Unexpected error has been detected.');
      }
      //an existing rule was updated
      else {
        if ($ret = SAVED_UPDATED) drupal_set_message('The rule has been modified.');
        else drupal_set_message('Unexpected error has been detected.');
      }
      //if everything was fine, print the rules with the newly added/modified one
      drupal_goto('admin/user/shib_auth/rules');
    }
  }
  // if something was wrang, print the edit form again
  return shib_auth_edit_form(array(0, '', '', '', 'Add rule', 0));
}//function shib_auth_new_rule()

/**
 * Edits an existing rule, containing he rule name, the server attrubite, the RegExp, and the role names
 *
 * @param $form_state contains all of the data of the form
 * @returns the edit form, with the fields already filled in
 */
function shib_auth_edit_rule($form_state, $id) {
  $form = array();
  // calls the edit form, with the fields of the existing rule
  if (is_int((int)$id)) {
    $rule = db_query("SELECT * FROM {shib_auth} WHERE id = %d", array($id));
    $db_entry = db_fetch_array($rule);
    return shib_auth_edit_form(
      array($db_entry['id'], $db_entry['field'], urldecode($db_entry['regexpression']), unserialize($db_entry['role']), 'Apply', $db_entry['sticky'])
    );
  }

}//function shib_auth_edit_rule()

/**
 * Admin users group membership validate
 *
 * @param $form data of the form
 * @param &$form_state contains all of the data of the form
 */
function shib_auth_admin_groups_validate($form, &$form_state) {

}//function shib_auth_admin_groups

/**
 * Admin users group membership submit
 *
 * @param $form data of the form
 * @param &$form_state contains all of the data of the form
 */
function shib_auth_admin_groups_submit($form, &$form_state) {
  variable_set('shib_auth_affilate_attrib', $form_state['values']['shib_auth_affilate_attrib']);
  drupal_set_message(t('Your changes are saved.'));
}//function shub_auth_groups_submit

