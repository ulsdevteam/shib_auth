<?php
// $Id: shib_auth_forms.inc,v 1.6.2.23 2009/02/14 06:26:23 shafter Exp $

function shib_auth_admin_general() {
  global $base_url;

  $form = array();

  $form['shib_handler_settings'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Shibboleth handler settings'),
    '#weight'      => -10, 
    '#collapsible' => FALSE,
  );

  $form['shib_attribute_settings'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Attribute settings'),
    '#weight'      => -5,
    '#collapsible' => FALSE,
  );


  $form['shib_handler_settings']['shib_auth_full_handler_url'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Shibboleth login handler URL'),
    '#default_value' => shib_auth_config('full_handler_url'),
    '#description'   => t('The URL can be absolute or relative to the server base url: http://www.example.com/Shibboleth.sso; /Shibboleth.sso'),
  );

  $form['shib_handler_settings']['shib_auth_full_logout_url'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Shibboleth logout handler URL'),
    '#default_value' => shib_auth_config('full_logout_url'),
    '#description'   => t('The URL can be absolute or relative to the server base url: http://www.example.com/Shibboleth.sso; /Shibboleth.sso'),
  );

  $form['shib_handler_settings']['shib_auth_link_text'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Shibboleth login link text'),
    '#default_value' => shib_auth_config('link_text'),
    '#description'   => t('The text of the login link. You can change this text on the Shibbolet login block settings form too!'),
  );

  $form['shib_handler_settings']['shib_auth_force_https'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Force HTTPS on login'),
    '#description'   => t('If the Shibboleth login handler uses Secure HTTP connections than the user
    will forced with this feature to return also to secure page (by using HTTPS) even the visited url
    was unsecure (HTTP).'),
    '#default_value' => shib_auth_config('force_https'),
  );

  $form['shib_attribute_settings']['shib_auth_username_variable'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Server variable for username'),
    '#default_value' => shib_auth_config('username_variable'),
  );

  $form['shib_attribute_settings']['shib_auth_email_variable'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Server variable for e-mail address'),
    '#default_value' => shib_auth_config('email_variable'),
  );

  $form['shib_attribute_settings']['shib_auth_define_username'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('User-defined usernames'),
    '#description'   => t('Allow users to set their Drupal usernames at first Shibboleth login. Note 
    that disabling this option only prevents new users from registering their 
    own username. Existing user-defined usernames will remain valid.'),
    '#default_value' => shib_auth_config('define_username'),
  );
  
  $form['shib_attribute_settings']['shib_auth_enable_custom_mail'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('User-defined e-mail addresses'),
    '#description'   => t('Ask users to set their contact email address at first login.
    Disabling this option will override contact address with the one, which was received from IdP.
    (In this case, missing e-mail address will result in fatal error.)'),
    '#default_value' => shib_auth_config('enable_custom_mail'),
  );

  $form['shib_attribute_settings']['shib_auth_account_linking'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Account linking'),
    '#description'   => t('Allow locally authenticated users to link their Drupal accounts to 
    federated logins. Note that disabling this option only prevents from 
    creating/removing associations, existing links will remain valid.'),
    '#default_value' => shib_auth_config('account_linking'),
  );

  $form['shib_attribute_settings']['shib_auth_account_linking_text'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Shibboleth account linking text'),
    '#default_value' => shib_auth_config('account_linking_text'),
    '#description'   => t('The text of the account linking link what is showing in the user change form.'),
  );

  $form['shib_attribute_debug'] = array(
    '#type'          => 'fieldset',
    '#title'         => 'Debugging options',
    '#weight'      => -1,
  );

  $form['shib_attribute_debug']['shib_auth_debug_state'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Enable DEBUG mode.'),
    '#default_value' => shib_auth_config('debug_state'),
  );

  $form['shib_attribute_debug']['shib_auth_debug_url'] = array(
    '#type' => 'textfield',
    '#title'         => t('DEBUG path prefix'),
    '#default_value' => shib_auth_config('debug_url'),
    '#description'   => t('For example use \'user/\' for display DEBUG messages on paths \'user/*\'!')
  );

  return system_settings_form($form);
} // function shib_auth_admin_general()

/**
 * Generate the custom e-mail and username provider form
 * @returns HTML text of the custom data form
 */
function shib_auth_custom_data() {
  global $user;
  $form = array();
  $auth_map_un_query = db_query("SELECT * FROM {shib_authmap} WHERE targeted_id='%s'", $_SERVER[shib_auth_config('username_variable')]);
  $authmap_username = db_fetch_array($auth_map_un_query);
  if ($authmap_username)
  {
    $form['consent'] = array(
        '#value' => t('The terms of agreement has been modified since your last login with id @targetedid. Please read it carefully, and click on Submit after your acceptance. Version you accepted: @accepted, document version: @documentver', array('@targetedid' => $authmap_username['targeted_id'], '@accepted' => $authmap_username['consentver'], '@documentver' => shib_auth_config('terms_ver') )),
      );
  }
  else {
    if (shib_auth_config('define_username') && !$user->uid) {
      $form['custom_username'] = array(
        '#type' => 'textfield',
        '#title' => t('Desired username'),
        '#default_value' => $_SERVER[shib_auth_config('username_variable')],
        '#size' => 60,
      );
      }
      else  {
        $form['custom_usernamem'] = array(
        '#value' => t('<b>Username:</b> <br>'),
        );
        $form['custom_username'] = array(
        '#value' => $_SERVER[shib_auth_config('username_variable')],
        '#suffix' => '<br>',
        );
      }
    if (shib_auth_config('enable_custom_mail') && !$user->uid) {
      $form['custom_mail'] = array(
        '#type' => 'textfield',
        '#title' => t('E-mail'),
        '#default_value' => $_SERVER[shib_auth_config('email_variable')],
        '#size' => 60,
      );
      }
      else  {
        $form['custom_mailm'] = array(
        '#value' => t('<b>E-mail:</b> <br>'),
        );
        $form['custom_mail'] = array(
        '#value' => $_SERVER[shib_auth_config('email_variable')],
        '#suffix' => '<br>',
        );
      }
  }
  if (shib_auth_config('terms_accept')) {
    global $base_url;
    if (drupal_substr(shib_auth_config('terms_url'), 0, 1) == '/')
      $url = $base_url.shib_auth_config('terms_url');
    else $url = shib_auth_config('terms_url');
    $form['accept'] = array(
    '#type'          => 'checkbox',
    '#title'         => 'I accept the <a href="'. $url
                   .'" target="_blank">terms and conditions</a>',
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Send'),
  );

  return $form;
} // function shib_auth_custom_data()

/**
 * Form validator
 * @param form form identifier
 * @param form_state $form_state contains all of the data of the form
 */
function shib_auth_custom_data_validate($form, &$form_state) {
  IF (shib_auth_config('enable_custom_mail'))
    if ($form_state['values']['custom_mail'] == '') {
      form_set_error('', t('You have to fill the \'E-mail\' field.'));
    }
    
  IF (shib_auth_config('define_username'))
    if ($form_state['values']['custom_username'] == '') {
      form_set_error('', t('You have to fill the \'Username\' field.'));
    }
   IF (shib_auth_config('terms_accept'))
    if ($form_state['values']['accept'] == FALSE) {
      form_set_error('', t('You have to accept Terms and conditions to proceed.'));
    }
} // shib_auth_custom_email_validate()


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

/**
 * Generate the administration form of the Shibboleth authentication module
 * @returns HTML text of the administration form
 */
function shib_auth_admin_advanced() {
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
    '#default_value' => shib_auth_config('is_passive'),
  );
  $form['shib_handler_settings']['shib_auth_forceauthn'] = array(
     '#type'          => 'checkbox',
    '#title'         => t('Enable ForceAuthN function'),
    '#description'   => t('Force users to re-authenticate'),
    '#default_value' => shib_auth_config('forceauthn'),
  );

  $form['shib_auth_auto_destroy_session'] = array(
    '#type' => 'fieldset',
    '#title' => t('Strict shibboleth session checking'),
    '#weight'      => -2,
  );

  $form['shib_auth_auto_destroy_session']['shib_auth_auto_destroy_session'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Destroy Drupal session when the Shibboleth session expires.'),
    '#default_value' => shib_auth_config('auto_destroy_session'),
  );

  $form['shib_logout_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Logout settings'),
    '#weight'      => -1,
  );

  $form['shib_logout_settings']['shib_auth_logout_url'] = array(
    '#type' => 'textfield',
    '#title' => t("URL to redirect to after logout"),
    '#default_value' => shib_auth_config('logout_url'),
    '#description' => t("The URL can be absolute or relative to the server base url. The relative paths will be automatically extended with the site base URL. Please note that, this won't work, if you are using SLO.")
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
    '#default_value' => shib_auth_config('terms_accept'),
  );

  $form['shib_terms_conditions_settings']['shib_auth_terms_url'] = array(
    '#type'          => 'textfield',
    '#title'         => t('URL of the document'),
    '#default_value' => shib_auth_config('terms_url'),
    '#description'   => t('If it is a drupal content, please start the link with /'),
    '#prefix' => '<table border= \'0\'><tr><td>',
    '#suffix' => '</td>',
  );

  $form['shib_terms_conditions_settings']['shib_auth_terms_ver'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Document version'),
    '#default_value' => shib_auth_config('terms_ver'),
    '#prefix' => '<td valign=top>',
    '#suffix' => '</td></tr></table>',
  );
  return system_settings_form($form);
} 
