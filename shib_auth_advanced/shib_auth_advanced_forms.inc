<?php
// $Id: shib_auth_forms.inc,v 1.6.2.23 2009/02/14 06:26:23 shafter Exp $

/**
 * Generate the administration form of the Shibboleth authentication module
 * @returns HTML text of the administration form
 */
function shib_auth_advanced_admin_advanced() {
  global $base_url;

  $form = array();

  $form['shib_handler_settings'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Advanced SAML2 settings'),
    '#weight'      => 0,
    '#collapsible' => FALSE,
  );
  
  $form['shib_handler_settings']['shib_auth_is_passive'] = array(
     '#type'          => 'checkbox',
    '#title'         => t('Enable IsPassive checking'),
    '#description'   => t('Enable IsPassive checking'),
    '#default_value' => variable_get('shib_auth_is_passive', FALSE),
  );
  $form['shib_handler_settings']['shib_auth_forceauthn'] = array(
     '#type'          => 'checkbox',
    '#title'         => t('Enable ForceAuthN function'),
    '#description'   => t('Force users to re-authenticate'),
    '#default_value' => variable_get('shib_auth_forceauthn', FALSE),
  );

  $form['shib_auth_auto_destroy_session'] = array(
    '#type' => 'fieldset',
    '#title' => t('Strict shibboleth session checking'),
    '#weight'      => -2,
  );

  $form['shib_auth_auto_destroy_session']['shib_auth_auto_destroy_session'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Destroy Drupal session when the Shibboleth session expires.'),
    '#default_value' => variable_get('shib_auth_auto_destroy_session', FALSE),
  );

  $form['shib_logout_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Logout settings'),
    '#weight'      => -1,
  );

  $form['shib_logout_settings']['shib_logout_url'] = array(
    '#type' => 'textfield',
    '#title' => t("URL to redirect to after logout"),
    '#default_value' => variable_get('shib_logout_url', ''),
    '#description' => t("The URL can be absolute or relative to the server base url. The relative paths will be automatically extended with the site base URL.")
  );

 $form['shib_terms_conditions_settings'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Terms and condition acception settings'),
    '#weight'      => -2,
    '#collapsible' => FALSE,
  );


  $form['shib_terms_conditions_settings']['shib_auth_terms_accept'] = array(
     '#type'          => 'checkbox',
    '#title'         => t('Force users to accept Terms and conditions'),
    '#description'   => t('Require acceptance of Terms and conditions every time, it changes'),
    '#default_value' => variable_get('shib_auth_terms_accept', FALSE),
  );

  $form['shib_terms_conditions_settings']['shib_auth_terms_url'] = array(
    '#type'          => 'textfield',
    '#title'         => t('URL of the document'),
    '#default_value' => variable_get('shib_auth_terms_url', '/'),
    '#description'   => t('If it is a node, it won\'t be accessible throughout the registration process'),
    '#prefix' => '<table border= \'0\'><tr><td>',
    '#suffix' => '</td>',
  );

  $form['shib_terms_conditions_settings']['shib_auth_terms_ver'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Document version'),
    '#default_value' => variable_get('shib_auth_terms_ver', ''),
    '#prefix' => '<td valign=top>',
    '#suffix' => '</td></tr></table>',
  );
  return system_settings_form($form);
} // function shib_auth_admin_advanced()


/**
 * Generate the shibboleth rule adding form
 *
 * @param $options contains the data, we want to fill the form with
 * @returns the edit form, with the fields already filled in with the elements of the options array
 */

/**
 * This function prevents drupal loading a cached page after shibboleth login
 */
function shib_login() {
  drupal_goto(substr($_GET['q'], 11));
}


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

