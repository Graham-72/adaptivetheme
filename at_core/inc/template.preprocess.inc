<?php
/**
 * IMPORTANT WARNING: DO NOT MODIFY THIS FILE.
 *
 * If you need to add or modify preprocess functions do it in your sub-theme.
 */

global $path_to_at_core;
// This holds a misc group of functions called by preprocess functions
include_once($path_to_at_core . '/inc/template.helpers.inc');
// Get our config arrys
include_once($path_to_at_core . '/inc/config.inc');

/**
 * Preprocess variables for html.tpl.php
 */
function adaptivetheme_preprocess_html(&$vars) {
  global $theme_key, $language, $path_to_at_core;

  $theme_name = $theme_key;
  $vars['base_path'] = base_path();
  $vars['path_to_adaptivetheme'] = $path_to_at_core;

  // AT no longer uses doctype switching or $rdf_profile, these maintain backwards
  // compatibility with pre 7.x-2.1 versions of html.tpl.php
  $vars['doctype'] = '<!DOCTYPE html>' . "\n";
  $vars['rdf_profile'] = '';

  // Clean up the lang attributes, great idea Zen
  $vars['html_attributes'] = 'lang="' . $language->language . '" dir="' . $language->dir . '"';

  // add extra classes if set
  if (theme_get_setting('extra_page_classes', $theme_name) === 1) {
    if (function_exists('locale')) {
      $vars['classes_array'][] = 'lang-' . $vars['language']->language;
    }

    // Classes for body element. Allows advanced theming based on context
    // (home page, node of certain type, etc.), cheers Zen.
    if (!$vars['is_front']) {
      // Add unique class for each page.
      $path = drupal_get_path_alias($_GET['q']);
      // Add unique class for each website section.
      list($section, ) = explode('/', $path, 2);
      $arg = explode('/', $_GET['q']);
      if ($arg[0] === 'node') {
        if (isset($arg[1]) && $arg[1] === 'add') {
          $section = 'node-add';
        }
        elseif (isset($arg[2]) && is_numeric($arg[1]) && ($arg[2] === 'edit' || $arg[2] === 'delete')) {
          $section = 'node-' . $arg[2];
        }
      }
      $vars['classes_array'][] = drupal_html_class('section-' . $section);
    }

    // Set classes for views and panels pages, thanks again Zen, you're amazing!
    $vars['menu_item'] = menu_get_item();
    switch ($vars['menu_item']['page_callback']) {
      case 'views_page':
        // Is this a Views page?
        $vars['classes_array'][] = 'page-views';
        break;
      case 'page_manager_page_execute':
      case 'page_manager_node_view':
      case 'page_manager_contact_site':
        // Is this a Panels page?
        $vars['classes_array'][] = 'page-panels';
        break;
    }
  }

  // add a body class if the site name is hidden
  if (theme_get_setting('toggle_name', $theme_name) === 0) {
    $vars['classes_array'][] = 'site-name-hidden';
  }

  // strip rubbish classes if extra classes not set
  if (theme_get_setting('extra_page_classes', $theme_name) === 0) {
    foreach ($vars['classes_array'] as $i => $class) {
      if (preg_match('/page*/', $class)) {
        unset($vars['classes_array'][$i]);
      }
    }
  }

  // Load Styles and JS
  //$theme_name = $theme_key;

  // Set the path to the directory where our CSS files are saved
  $path = "public://at_css";
  $file = '';

  // Set a variable for the mode
  $mode = theme_get_setting('production_mode');

  /**
   * Responsive Stylesheet Loader: Development Mode
   *
   * We only load these files if:
   * - 1: We're not in production mode
   * - 2: Responsiveness is not disabled
   *
   * When in production mode these files are aggregated into one file
   * and loaded using load_subtheme_responsive_styles().
   *
   * You can disable responsive capability using the theme settings - look under
   * the Global settings. Do not delete or alter this - use the theme setting!
   */
   if ($mode === 0) {
     if (theme_get_setting('disable_responsive_styles', $theme_name) === 0) {

      // Get the media query for each device group.
      $smartphone_portrait_media_query  = theme_get_setting('smartphone_portrait_media_query', $theme_name);
      $smartphone_landscape_media_query = theme_get_setting('smartphone_landscape_media_query', $theme_name);
      $tablet_portrait_media_query      = theme_get_setting('tablet_portrait_media_query', $theme_name);
      $tablet_landscape_media_query     = theme_get_setting('tablet_landscape_media_query', $theme_name);
      $desktop_media_query              = theme_get_setting('bigscreen_media_query', $theme_name);

      // Next build a keyed array of CSS file-names as key, and media query as value.
      // The file names must match the files in your themes CSS directory, if
      // you change them you must update to match.
      $files = array(
        'responsive.smartphone.portrait'  => $smartphone_portrait_media_query,
        'responsive.smartphone.landscape' => $smartphone_landscape_media_query,
        'responsive.tablet.portrait'      => $tablet_portrait_media_query,
        'responsive.tablet.landscape'     => $tablet_landscape_media_query,
        'responsive.desktop'              => $desktop_media_query,
      );

      // Add the cascading stylesheet if enabled
      if (theme_get_setting('enable_cascading_media_queries', $theme_name) === 1) {
        $cascade_media_query = theme_get_setting('cascade_media_query', $theme_name);
        $item = array('responsive.cascade' => $cascade_media_query);
        $files = array_merge($item, $files);
      }

      // Loop over the files array and load each CSS.
      // load_subtheme_responsive_styles() takes 3 arguements:
      // - $css_file - the name of the css file
      // - $media_query - the media query from theme settings to use for the media attribute
      // - $theme_name - to generate the path-to-theme for drupal_add_css()
      foreach ($files as $key => $value) {
        $css_file = $key . '.css';
        $media_query = $value;
        load_subtheme_responsive_styles($css_file, $media_query, $theme_name);
      }
    }
  }

  // In Production mode we only load one file for responsive styles.
  // We use a media query to prevent non media query browsers from downloading it.
  // The embedded media queries in this file take precedence always.
  if ($mode === 1) {
    $file = $theme_name . '.responsive.styles.css';
    $media_query = 'only screen and (min-width:1px)';
    if (!empty($file)) {
      $filepath = "$path/$file";
      drupal_add_css($filepath, array(
        'preprocess' =>  variable_get('preprocess_css', '') == 1 ? TRUE : FALSE,
        'group' => CSS_THEME,
        'media' => $media_query,
        'weight' => 99,
        'every_page' => TRUE,
        )
      );
    }
  }

  // In layout.inc we built 3 layout stylesheets:
  // 1. themeName.default.layout.css - this always loads, so we just load it
  // 2. themeName.responsive.layout.css - only loads if responsive capability is enabled
  // 3. themeName.lt-ie9.layout.css - only loads if responsive capability is enabled
  //
  // Now we need to load them, but only under certain conditions:
  // a. Is the default layout the standard layout?
  // b. Is the responsive capability disabled?
  //
  // If either a or b equate to TRUE, then we are not doing mobile first
  // If a is FALSE then this is mobile first
  // If b is FALSE then we are doing responsive design

  // Load the default layout, but only if Standard layout is default, otherwise we do nothing
  if (theme_get_setting('global_default_layout', $theme_name) === 'standard-layout') {
    $file = $theme_name . '.default.layout.css';
    if (!empty($file)) {
      $filepath = "$path/$file";
      drupal_add_css($filepath, array(
        'preprocess' => TRUE,
        'group' => CSS_THEME,
        'media' => 'screen, handheld',
        'every_page' => TRUE,
        )
      );
    }
  }

  // Next we check if we need to load the responsive layout
  if (theme_get_setting('disable_responsive_styles', $theme_name) === 0) {
    $file = $theme_name . '.responsive.layout.css';
    if (!empty($file)) {
      $filepath = "$path/$file";
      drupal_add_css($filepath, array(
        'preprocess' => variable_get('preprocess_css', '') === 1 ? TRUE : FALSE,
        'group' => CSS_THEME,
        'media' => 'screen, handheld',
        'every_page' => TRUE,
        )
      );
    }
  }

  // If load_respondjs is enabled, load it
  if (theme_get_setting('load_respondjs', $theme_name) === 1) {
    drupal_add_js($path_to_at_core . '/js/respond.js',
      array(
        'type' => 'file',
        'scope' => 'footer',
        'group' => JS_THEME,
        'preprocess' => TRUE,
        'cache' => TRUE,
      )
    );
  }

  // The lt-ie9.layout.css stylesheet loads under one very specific set of conditions:
  // 1. Responsive capabilities are ON
  // 2. Mobile first is ON
  // 3. Respond.js is OFF
  // Note this excludes IEMobile 7
  if (theme_get_setting('disable_responsive_styles', $theme_name) === 0) {
    if (theme_get_setting('global_default_layout', $theme_name) !== 'standard-layout') {
      if (theme_get_setting('load_respondjs', $theme_name) === 0) {
        $ie_file = $theme_name . '.lt-ie9.layout.css';
        if (!empty($ie_file)) {
          $ie_filepath = "$path/$ie_file";
          drupal_add_css($ie_filepath, array(
            'group' => CSS_THEME,
            'media' => 'screen',
            'browsers' => array(
              'IE' => '(lt IE 9)&(!IEMobile 7)',
              '!IE' => FALSE,
              ),
            'preprocess' => TRUE,
            )
          );
        }
      }
    }
  }

  // Load micro CSS files and body classes if settings are active
  $settings_css = array();
  // Headings
  if(theme_get_setting('enable_heading_settings', $theme_name) === 1) {
    $settings_css[] = 'at.settings.style.headings.css';
    $settings_array = array(
      'page_title_case',
      'page_title_weight',
      'page_title_alignment',
      'page_title_shadow',
      'node_title_case',
      'node_title_weight',
      'node_title_alignment',
      'node_title_shadow',
      'comment_title_case',
      'comment_title_weight',
      'comment_title_alignment',
      'comment_title_shadow',
      'block_title_case',
      'block_title_weight',
      'block_title_alignment',
      'block_title_shadow',
    );
    foreach ($settings_array as $setting) {
      if ($this_setting = theme_get_setting($setting, $theme_name)) {
        $vars['classes_array'][] = $this_setting;
      }
    }
  }
  // Images
  if(theme_get_setting('enable_image_settings', $theme_name) === 1) {
    $settings_css[] = 'at.settings.style.image.css';
    $settings_array = array(
      'image_caption_full',
      'image_caption_teaser',
      'image_alignment',
    );
    foreach ($settings_array as $setting) {
      if ($this_setting = theme_get_setting($setting, $theme_name)) {
        $vars['classes_array'][] = $this_setting;
      }
    }
  }
  // Login block
  if (theme_get_setting('enable_loginblock_settings.css', $theme_name) === 1) {
    $settings_css[] = 'at.settings.style.login';
  }
  $filepath = $path_to_at_core . '/css/';
  foreach ($settings_css as $file) {
    drupal_add_css($filepath . $file, array(
      'preprocess' => TRUE,
      'group' => CSS_THEME,
      'media' => 'screen, handheld',
      'every_page' => TRUE,
      )
    );
  }

  // Font loader
  // Adaptivetheme can load websafe fonts, Google fonts
  // custom font stacks and integrate with @font-your-face module.
  // All can be configured in the "Fonts" theme settings.
  if (theme_get_setting('enable_font_settings', $theme_name) === 1) {
    // Pull in the font lists
    include_once($path_to_at_core . '/inc/font.lists.inc');

    $used_fonts = array();
    $font_elements = font_elements();
    foreach ($font_elements as $key => $value) {
      $setting = $value['setting'];
      $element = $value['element'];
      // Check if any elements are set to use a google font
      if (theme_get_setting($setting . '_type', $theme_name) === 'gwf') {
        // Get the theme setting
        $setting = theme_get_setting($setting . '_gwf', $theme_name);
        // strip the element prefix
        $font = str_replace($element . '-', '', $setting);
        // replace hyphens with spaces, now we have a lower case font name, no good,
        // Google won't like it and will return the dreaded "font not supported" error
        $used_fonts[] = str_replace('-', ' ', $font);
      }
    }
    if (!empty($used_fonts)) {
      // Get the full list of font names, remember Google fonts are case sensitive, and
      // you cannot just uppercase/ucfirst, font names are not consistant
      $google_fonts = google_font_names();
      // Populate a variable with lower cased font names
      $google_fonts_lower_case = unserialize(strtolower(serialize($google_fonts)));
      // Now key the array with our lower case font names, and set that to a variable
      $google_fonts_lower_case_keys = array_combine($google_fonts_lower_case, $google_fonts);
      $used_google_fonts = array();
      // Now we can find a match, comparing our lowercase $used_fonts with the lower case keys
      foreach ($google_fonts_lower_case_keys as $k => $v) {
        if (in_array($k, $used_fonts)) {
          $used_google_fonts[] = $v; // at last, we have the right font name/s
        }
      }
      // The same font can be used multiple times, but we only need to load it once
      $array = array_unique($used_google_fonts);
      // Implode the array, seperate font names with a pipe
      $google_fonts = trim(implode('|', $array));
      drupal_add_css('//fonts.googleapis.com/css?family=' . $google_fonts, array(
        'group' => CSS_THEME,
        'type' => 'external',
        'weight' => -100,
        'preprocess' => FALSE,
        )
      );
    }

    // Load the fonts CSS from public files
    $fonts_css = $theme_name . '_font_families.css';
    $fonts_css_filepath = "$path/$fonts_css";
    drupal_add_css($fonts_css_filepath, array(
      'preprocess' => TRUE,
      'group' => CSS_THEME,
      //'weight' => -100,
      'media' => 'screen, handheld',
      'every_page' => TRUE,
      )
    );
  }

  // Finally we do some debugging/development stuff
  // add a body class for exposing region
  if (theme_get_setting('expose_regions', $theme_name) === 1) {
    $vars['classes_array'][] = 'debug-regions';
    $vars['classes_array'][] = 'debug-panels';
    drupal_add_css($path_to_at_core . '/css/debug-regions.css');
  }
  // Load window size bookmarklet
  if (theme_get_setting('show_window_size', $theme_name) === 1) {
    drupal_add_js($path_to_at_core . '/js/window-size.js',
      array(
        'type' => 'file',
        'scope' => 'footer',
        'group' => JS_THEME,
        'preprocess' => TRUE,
        'cache' => TRUE,
      )
    );
  }
}


/**
 * Preprocess variables for page.tpl.php
 */
function adaptivetheme_preprocess_page(&$vars) {
  global $theme_key;
  $theme_name = $theme_key;

  // Set up logo element
  $vars['site_logo'] = '';
  if (theme_get_setting('toggle_logo', $theme_name) === 1) {
    $logo_path = check_url($vars['logo']);
    $logo_alt = check_plain(variable_get('site_name', 'Site logo'));
    $logo_vars = array('path' => $logo_path, 'alt' => $logo_alt, 'attributes' => array('class' => 'site-logo'));
    $logo_img = theme('image', $logo_vars);
    $vars['site_logo'] = $logo_img ? l($logo_img, '<front>', array(
      'attributes' => array(
        'title' => t('Home page')
      ),
      'html' => TRUE,
      )
    ) : '';
  }
  // Maintain backwards compatibility with 7.x-2.x sub-themes
  $vars['logo_img'] = isset($logo_img) ? $logo_img : '';
  $vars['linked_site_logo'] = $vars['site_logo'];

  // Hide site name if toggled off
  if (theme_get_setting('toggle_name', $theme_name) === 0) {
    $vars['visibility'] = 'element-invisible';
    $vars['hide_site_name'] = TRUE;
  }
  else {
    $vars['visibility'] = '';
    $vars['hide_site_name'] = FALSE;
  }
  $sitename = filter_xss_admin(variable_get('site_name', 'Drupal')); /* Convert entitles if used, cunning... */
  $vars['site_name'] = $sitename ? l($sitename, '<front>', array(
    'attributes' => array(
      'title' => t('Home page')),
    )
  ) : '';

  // Build a variable for the main menu
  if (isset($vars['main_menu'])) {
    $text = t('Main menu');
    if (module_exists('block')) {
      $text = block_get_blocks_by_region('menu_bar') ? t('Navigation') : t('Main menu');
    }
    $vars['primary_navigation'] = theme('links', array(
      'links' => $vars['main_menu'],
      'attributes' => array(
        'class' => array('menu', 'primary-menu', 'clearfix'),
       ),
      'heading' => array(
        'text' => $text,
        'level' => 'h2',
        'class' => array('element-invisible'),
      )
    ));
  }

  // Build a variable for the secondary menu
  if (isset($vars['secondary_menu'])) {
    $vars['secondary_navigation'] = theme('links', array(
      'links' => $vars['secondary_menu'],
      'attributes' => array(
        'class' => array('menu', 'secondary-menu', 'clearfix'),
      ),
      'heading' => array(
        'text' => t('Secondary navigation'),
        'level' => 'h2',
        'class' => array('element-invisible'),
      )
    ));
  }

  // Build variables for Primary and Secondary local tasks
  $vars['primary_local_tasks'] = menu_primary_local_tasks();
  $vars['secondary_local_tasks'] = menu_secondary_local_tasks();

  // Add back the $search_box var to D7
  if (module_exists('search')) {
    $search_box = drupal_get_form('search_form');
    $vars['search_box'] = '<div id="search-box">' . drupal_render($search_box) . '</div>';
  }

  // Remove the infernal feed icons
  if (theme_get_setting('feed_icons_hide', $theme_name) === 1) {
    $vars['feed_icons'] = '';
  }

  // Remove block system main, just from the front page
  if (drupal_is_front_page()) {
    if (theme_get_setting('unset_block_system_main_front', $theme_name) === 1) {
      $vars['page']['content']['system_main'] = '';
    }
  }

  // Attribution variable used in admin theme and some others
  $vars['attribution'] = "<small id=\"attribution\"><a href=\"http://adaptivethemes.com\">Premium Drupal Themes</a></small>";

  // Work-around a stupid bug in Drupal 7
  if (arg(0) === 'user' && arg(1) === 'login') {
    drupal_set_title(t('User login'));
  }
  if (arg(0) === 'user' && arg(1) === 'password') {
    drupal_set_title(t('Request new password'));
  }
  if (arg(0) === 'user' && arg(1) === 'register') {
    drupal_set_title(t('Create new account'));
  }
}

/**
 * Preprocess variables for region.tpl.php
 */
function adaptivetheme_preprocess_region(&$vars) {
  // Add a "sidebar" class to sidebar regions
  if (strpos($vars['region'], 'sidebar_') === 0) {
    $vars['classes_array'][] = 'sidebar';
  }
  // Add classes to conditional stack regions
  $conditional_stack_regions = array(
    'three_25_25_50_top',
    'three_25_25_50_bottom',
    'three_25_50_25_top',
    'three_25_50_25_bottom',
    'three_50_25_25_top',
    'three_50_25_25_bottom',
    'three_33_top',
    'three_33_bottom',
    'two_50_top',
    'two_50_bottom',
    'two_33_66_top',
    'two_33_66_bottom',
    'two_66_33_top',
    'two_66_33_bottom',
    'two_brick_top',
    'two_brick_middle',
    'two_brick_bottom',
    'threecol_inset_left_top',
    'threecol_inset_left_bottom',
    'threecol_inset_right_top',
    'threecol_inset_right_bottom',
  );
  if (in_array($vars['region'], $conditional_stack_regions)){
    $vars['classes_array'][] = 'region-conditional-stack';
  }
}

/**
 * Preprocess variables for block.tpl.php
 */
function adaptivetheme_preprocess_block(&$vars) {
  global $theme_key;
  $theme_name = $theme_key;

  // The menu bar region gets special treatment for the block template
  if ($vars['block']->region === 'menu_bar') {
    // Hide titles, very unlikey to want it show, ever
    $vars['title_attributes_array']['class'][] = 'element-invisible';
    $vars['classes_array'][] = 'menu-wrapper';
  }
  
  

  // Add extra classes if required
  if (theme_get_setting('extra_block_classes', $theme_name) === 1) {
    // Zebra
    $vars['classes_array'][] = $vars['block_zebra'];
    // Position?
    if ($vars['block_id'] === 1) {
      $vars['classes_array'][] = 'first';
    }
    if (isset($vars['block']->last_in_region)) {
      $vars['classes_array'][] = 'last';
    }
    // Count
    $vars['classes_array'][] = 'block-count-' . $vars['id'];
    // Region
    $vars['classes_array'][] = drupal_html_class('block-region-' . $vars['block']->region);
    // Delta
    $vars['classes_array'][] = drupal_html_class('block-' . $vars['block']->delta);
  }

  // Add classes to theme the horizontal block option
  if (theme_get_setting('horizontal_login_block', $theme_name) === 1 && $vars['block']->module === 'user' && $vars['block']->delta === 'login') {
    $vars['classes_array'][] = 'lb-h';
  }
  if (theme_get_setting('slider_login_block', $theme_name) === 1 && $vars['block']->module === 'user' && $vars['block']->delta === 'login') {
    $vars['classes_array'][] = 'lb-s';
  }

  // Give our block titles and content some class
  $vars['title_attributes_array']['class'][] = 'block-title';
  $vars['content_attributes_array']['class'][] = 'block-content';
  $vars['content_attributes_array']['class'][] = 'content';

  // Add ARIA Roles to blocks, thank-you to Jacine http://drupal.org/user/88931 for cleaning up this code :)
  $roles = array(
    'complementary' => array(
      'aggregator',
      'help',
      'locale',
      'poll',
      'profile',
      'node' => array('syndicate'),
      'system' => array('powered-by', 'help'),
      'user' => array('new', 'online'),
    ),
    'navigation' => array(
      'blog',
      'book',
      'comment',
      'forum',
      'menu',
      'menu_block',
      'node' => array('recent'),
      'shortcut',
      'statistics',
      'system' => array_keys(menu_list_system_menus()),
      'superfish',
      'nice_menus',
    ),
    'search' => array(
      'search',
    ),
    'form' => array(
      'user' => array('login'),
    ),
  );
  foreach ($roles as $role => $module) {
    if (!is_array($role) && in_array($vars['block']->module, $module)) {
      $vars['attributes_array']['role'] = "$role";
    }
    elseif (is_array($role)) {
      foreach ($role as $module => $delta) {
        if ($vars['block']->module == $module && in_array($vars['block']->delta, $delta)) {
          $vars['attributes_array']['role'] = "$role";
        }
      }
    }
  }
}


/**
 * Preprocess variables for node.tpl.php
 */
function adaptivetheme_preprocess_node(&$vars) {
  global $theme_key;
  $theme_name = $theme_key;

  // Extra classes if required
  if (theme_get_setting('extra_article_classes', $theme_name)) {
    // Zebra
    $vars['classes_array'][] = $vars['zebra'];
    // Langauge
    if (module_exists('translation')) {
      if ($vars['node']->language) {
        $vars['classes_array'][] = 'node-lang-' . $vars['node']->language;
      }
    }
    // User picture?
    if ($vars['display_submitted'] && !empty($vars['picture'])) {
      $vars['classes_array'][] = 'node-with-picture';
    }
    // Class for each view mode, core assumes we only need to target teasers but neglects custom view modes or full
    if ($vars['view_mode'] !== 'teaser') {
      $vars['classes_array'][] = drupal_html_class('node-' . $vars['view_mode']);
    }
  }

  // ARIA Role
  $vars['attributes_array']['role'][] = 'article';

  // Title and content classes
  $vars['title_attributes_array']['class'][] = 'node-title';
  $vars['content_attributes_array']['class'][] = 'node-content';

  // Build the submitted by and time elements
  $vars['datetime'] = format_date($vars['created'], 'custom', 'Y-m-d\TH:i:s\Z'); // PHP 'c' format is not proper ISO8601, we need to build it
  if (variable_get('node_submitted_' . $vars['node']->type, TRUE)) {
    $vars['submitted'] = t('Submitted by !username on !datetime',
      array(
        '!username' => $vars['name'],
        '!datetime' => '<time datetime="' . $vars['datetime'] . '" pubdate="pubdate">' . $vars['date'] . '</time>',
      )
    );
  }
  else {
    $vars['submitted'] = '';
  }

  // Unpublised?
  $vars['unpublished'] = '';
  if (!$vars['status']) {
    $vars['unpublished'] = '<div class="unpublished">' . t('Unpublished') . '</div>';
  }
}

/**
 * Preprocess variables for comment.tpl.php
 */
function adaptivetheme_preprocess_comment(&$vars) {
  global $theme_key;
  $theme_name = $theme_key;
  // Extra comment classes if required
  if (theme_get_setting('extra_comment_classes', $theme_name) === 1) {
    // Zebra
    $vars['classes_array'][] = $vars['zebra'];
    // Position?
    if ($vars['id'] === 1) {
      $vars['classes_array'][] = 'first';
    }
    if ($vars['id'] === $vars['node']->comment_count) {
      $vars['classes_array'][] = 'last';
    }
    // Title hidden?
    if (theme_get_setting('comments_hide_title', $theme_name) === 1) {
      $vars['classes_array'][] = 'comment-title-hidden';
    }
    // User picture?
    if (isset($vars['picture'])) {
      $vars['classes_array'][] = 'comment-with-picture';
    }
    // Signature?
    if (isset($vars['signature'])) {
      $vars['classes_array'][] = 'comment-with-signature';
    }
  }

  // Classes for comment title
  $vars['title_attributes_array']['class'][] = 'comment-title';
  // Title hidden?
  if (theme_get_setting('comments_hide_title', $theme_name) === 1) {
    $vars['title_attributes_array']['class'][] = 'element-invisible';
  }

  // Classes for comment content
  $vars['content_attributes_array']['class'][] = 'comment-content';

  // Build the submitted by and time elements
  $uri = entity_uri('comment', $vars['comment']);
  $uri['options'] += array('attributes' => array('rel' => 'bookmark'));
  $vars['title'] = l($vars['comment']->subject, $uri['path'], $uri['options']);
  $vars['permalink'] = l(t('Permalink'), $uri['path'], $uri['options']); // Permalinks are embedded in the time element, aka Wordpress
  $vars['created'] = '<span class="date-time permalink">' . l($vars['created'], $uri['path'], $uri['options']) . '</span>';
  $vars['datetime'] = format_date($vars['comment']->created, 'custom', 'Y-m-d\TH:i:s\Z'); // Generate the timestamp, PHP "c" format is wrong
  $vars['submitted'] = t('Submitted by !username on !datetime',
    array(
      '!username' => $vars['author'],
      '!datetime' => '<time datetime="' . $vars['datetime'] . '" pubdate="pubdate">' . $vars['created'] . '</time>',
    )
  );

  // Unpublished?
  $vars['unpublished'] = '';
  if ($vars['status'] === 'comment-unpublished') {
    $vars['unpublished'] = '<div class="unpublished">' . t('Unpublished') . '</div>';
  }
}

/**
 * Preprocess variables for the search block form.
 */
function adaptivetheme_preprocess_search_block_form(&$vars) {
  // Changes the search form to use the "search" input element attribute
  $vars['search_form'] = str_replace('type="text"', 'type="search"', $vars['search_form']);
}

/**
 * Preprocess variables for search-result.tpl.php
 */
function adaptivetheme_preprocess_search_result(&$vars) {
  global $theme_key;
  $theme_name = $theme_key;

  // Modify search result view based on theme settings
  $result = $vars['result'];
  $vars['url'] = check_url($result['link']);
  $vars['title'] = check_plain($result['title']);
  $vars['snippet'] = '';
  if (!empty($result['snippet']) && theme_get_setting('search_snippet', $theme_name) === 1) {
    $vars['snippet'] = $result['snippet'];
  }
  $info = array();
  if (!empty($result['type']) && theme_get_setting('search_info_type', $theme_name) === 1) {
    $info['type'] = check_plain($result['type']);
  }
  if (!empty($result['user']) && theme_get_setting('search_info_user', $theme_name) === 1) {
    $info['user'] = $result['user'];
  }
  if (!empty($result['date']) && theme_get_setting('search_info_date', $theme_name) === 1) {
    $info['date'] = format_date($result['date'], 'small');
  }
  if (isset($result['extra']) && is_array($result['extra'])) {
    if (!empty($result['extra'][0]) && theme_get_setting('search_info_comment', $theme_name) === 1) {
      $info['comment'] = $result['extra'][0];
    }
    if (!empty($result['extra'][1]) && theme_get_setting('search_info_upload', $theme_name) === 1) {
      $info['upload'] = $result['extra'][1];
    }
  }
  $vars['info_split'] = $info;
  $vars['info'] = implode(' - ', $info);
  $vars['info_separator'] = filter_xss(theme_get_setting('search_info_separator', $theme_name));
}

/**
 * Preprocess variables for field.tpl.php
 */
function adaptivetheme_preprocess_field(&$vars) {
  global $theme_key;
  $theme_name = $theme_key;

  $element = $vars['element'];

  // add a view mode class to fields
  $vars['classes_array'][] = 'view-mode-' . $element['#view_mode'];

  // provide an variable we can use to test the view mode inside templates
  $vars['field_view_mode'] = $element['#view_mode'] ? $element['#view_mode'] : '';

  // enable captioning capability for image fields
  if ($element['#field_type'] === 'image') {
    $vars['image_caption_teaser'] = FALSE;
    $vars['image_caption_full'] = FALSE;
    if (theme_get_setting('image_caption_teaser', $theme_name) === 1) {
      $vars['image_caption_teaser'] = TRUE;
    }
    if (theme_get_setting('image_caption_full', $theme_name) === 1) {
      $vars['image_caption_full'] = TRUE;
    }
  }
}

/**
 * Preprocess variables for aggregator-item.tpl.php
 */
function adaptivetheme_preprocess_aggregator_item(&$vars) {
  $item = $vars['item'];
  $vars['datetime'] = format_date($item->timestamp, 'custom', 'Y-m-d\TH:i:s\Z');
}

/**
 * Preprocess variables for maintenance-page.tpl.php
 */
function adaptivetheme_preprocess_maintenance_page(&$vars) {
  $vars['attribution'] = "<small id=\"attribution\"><a href=\"http://adaptivethemes.com\">Premium Drupal Themes</a></small>";
}