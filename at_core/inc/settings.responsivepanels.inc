<?php
/**
 * IMPORTANT WARNING: DO NOT MODIFY THIS FILE.
 *
 * Build the settings form for responsive panels. This gets its data
 * from responsive_panels() in config.inc
 */
  $form['at-layout']['panels'] = array(
    '#type' => 'fieldset',
    '#title' => t('Responsive Panels'),
  );
  $form['at-layout']['panels']['panels-wrapper'] = array(
    '#type' => 'fieldset',
    '#attributes' => array('class' => array('panel-option-lists')),
    '#description' => t('<h3>Responsive Panels</h3><p>These settings apply to both <a href="!gpanels_link" target="_blank">Gpanels</a> and <a href="!panels_link" target="_blank">Panels module</a> layouts.</p><p><strong>Usage:</strong> select layout options for each mobile device orientation.</p>', array('!panels_link' => 'http://drupal.org/project/panels', '!gpanels_link' => 'http://adaptivethemes.com/documentation/using-gpanels')),
  );

  // Hard code the devices, this is no problem as far as I am concerned
  $devices = array('tablet_landscape','tablet_portrait','smartphone_landscape');
  // Get the configuration array for panel layouts
  $rp = responsive_panels();

  foreach ($devices as $device) {
    //dsm($device);
    $title = str_replace('_', ' ', ucfirst($device));
    $form['at-layout']['panels']['panels-wrapper'][$device] = array(
      '#type' => 'fieldset',
      '#title' => t($title),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    foreach ($rp as $key => $value) {
      //dsm($key); // type such as two, tree, four
      $type = $key;
      $form['at-layout']['panels']['panels-wrapper'][$device][$type] = array(
        '#type' => 'fieldset',
        '#title' => t($key),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      foreach ($value as $key => $value) {
        //dsm($value); // array of settings for each subtype
        //dsm($device .'_'. $value['setting']); // theme setting = device + setting
        //dsm($value['options']); // array of options for each subtype
        $form['at-layout']['panels']['panels-wrapper'][$device][$type][$device .'_'. $value['setting']] = array(
          '#type' => 'radios',
          '#title' => t($value['title']),
          '#default_value' => theme_get_setting($device .'_'. $value['setting']),
          '#options' => $value['options'],
        );
      }
    }
  }