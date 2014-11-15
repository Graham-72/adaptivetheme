<?php

use Drupal\Core\Config\Config;

use Drupal\at_core\Theme\ThemeInfo;
use Drupal\at_core\Theme\ThemeSettingsConfig;
use Drupal\at_core\Layout\LayoutGenerator;

use Drupal\at_core\File\DirectoryOperations;

use Drupal\at_core\Layout\Layout;
use Drupal\at_core\Breakpoints\ATBreakpoints;

/**
 * Implimentation of hook_form_system_theme_settings_alter()
 *
 * @param $form
 *   Nested array of form elements that comprise the form.
 *
 * @param $form_state
 *   A keyed array containing the current state of the form.
 */
function at_core_form_system_theme_settings_alter(&$form, &$form_state) {

  // Set the theme name.
  $build_info = $form_state->getBuildInfo();
  $theme = $build_info['args'][0];

  // Instantiate our Theme info object.
  $themeInfo = new ThemeInfo($theme);
  $getThemeInfo = $themeInfo->getThemeInfo('info');

  // Get this themes config settings
  $config = \Drupal::config($theme . '.settings')->get('settings');

  // Common paths.
  $at_core_path  = drupal_get_path('theme', 'at_core');
  $subtheme_path = drupal_get_path('theme', $theme);

  // Path to save generated CSS files.
  $directoryOperations = new DirectoryOperations();
  $generated_files_path = $directoryOperations->directoryPrepare($backup_file_path = array($subtheme_path, '/css/generated'));

  // Get the active themes regions so we can use this in
  // various other places.
  $theme_regions = system_region_list($theme, $show = REGIONS_VISIBLE);

  // Active themes active blocks
  $theme_blocks = _block_rehash($theme);

  // Check for breakpoints module and set a warning and a flag to disable much of the theme settings if its not available
  $breakpoints_module = \Drupal::moduleHandler()->moduleExists('breakpoint');

  if ($breakpoints_module == TRUE) {
    // Get breakpoint groups.
    $breakpoint_groups = \Drupal::service('breakpoint.manager')->getGroups();

    // Set breakpoint options, we use these in layout and other extensions like Responsive menus.
    foreach ($breakpoint_groups as $group_key => $group_values) {
      $breakpoints[$group_key] = \Drupal::service('breakpoint.manager')->getBreakpointsByGroup($group_values);
    }

    foreach($breakpoints as $group => $breakpoint_values)  {
      if ($breakpoint_values !== array()) {
        $breakpoint_options[$group] = $group;
      }
    }
  }
  else {
    drupal_set_message(t('Adaptivetheme requires the <b>Breakpoint module</b>. Open the <a href="!extendpage" target="_blank">Extend</a> page and enable Breakpoint.', array('!extendpage' => base_path() . 'admin/modules')), 'warning');
  }

  // Get node types (bundles).
  $node_types = node_type_get_types();

  // View or "Display modes", the search display mode is still problematic so we will exclude it for now,
  // please see: https://drupal.org/node/1166114
  //$node_view_modes = \Drupal::entityManager()->getViewModeOptions('node', TRUE);
  $node_view_modes = \Drupal::entityManager()->getViewModes('node');

  // Unset unwanted view modes
  unset($node_view_modes['rss']);
  unset($node_view_modes['search_index']);
  unset($node_view_modes['search_result']);


  // Attached required CSS and JS libraries and files.
  $form['#attached'] = array(
    'css' => array(
      $at_core_path . '/stylesheets/css/appearance.css',
    ),
    'js' => array(
      $at_core_path . '/scripts/appearance.js',
    ),
  );

  // AT Core
  if ($theme == 'at_core') {
    $form['at_core']['message'] = array(
      '#type' => 'container',
      '#markup' => t('AT Core has no configuration and cannot be used as a front end theme - it is a base them only. Use the <b>AT Theme Generator</b> to generate or clone a theme to get started.'),
    );

    // Hide form items.
    $form['theme_settings']['#attributes']['class'] = array('visually-hidden');
    $form['logo']['#attributes']['class'] = array('visually-hidden');
    $form['favicon']['#attributes']['class'] = array('visually-hidden');
    $form['actions']['#attributes']['class'] = array('visually-hidden');
  }

  // AT Subtheme
  if (isset($getThemeInfo['subtheme type'])) {

    if ($getThemeInfo['subtheme type'] !== 'at_generator') {

      // Pass in the generated files path to values and settings.
      $form['at']['settings_generated_files_path'] = array(
        '#type' => 'hidden',
        '#value' => $generated_files_path,
      );

      // Extension settings.
      include_once($at_core_path . '/forms/ext/extension_settings.php');

      // Layouts.
      include_once($at_core_path . '/forms/layout/layouts.php');

      // Basic settings - move into details wrapper and collapse.
      $form['basic_settings'] = array(
        '#type' => 'details',
        '#title' => 'Basic Settings',
        '#open' => FALSE,
        //'#weight' => 100,
      );

      $form['theme_settings']['#open'] = FALSE;
      $form['theme_settings']['#group'] = 'basic_settings';
      $form['logo']['#open'] = FALSE;
      $form['logo']['#group'] = 'basic_settings';
      $form['favicon']['#open'] = FALSE;
      $form['favicon']['#group'] = 'basic_settings';

      // buttons don't work with #group, move it the hard way.
      $form['actions']['#type'] = $form['basic_settings']['actions']['#type'] = 'actions';
      $form['actions']['submit']['#type'] = $form['basic_settings']['actions']['submit']['#type'] = 'submit';
      $form['actions']['submit']['#value'] = $form['basic_settings']['actions']['submit']['#value'] = t('Save basic settings');
      $form['actions']['submit']['#button_type'] = $form['basic_settings']['actions']['submit']['#button_type'] = 'primary';
      unset($form['actions']);
    }
  }

  // Modify the color scheme form.
  if (\Drupal::moduleHandler()->moduleExists('color')) {
    if (isset($build_info['args'][0]) && ($theme = $build_info['args'][0]) && color_get_info($theme) && function_exists('gd_info')) {
      $form['#process'][] = 'at_core_make_collapsible';
    }
  }
}

// Helper function to modify the color scheme form.
function at_core_make_collapsible($form) {
  $form['color']['#open'] = FALSE;

  $form['color']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('submit--color-scheme')),
  );

  $form['color']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save color scheme'),
    '#button_type' => 'primary',
    '#weight' => 100,
  );

  return $form;
}
