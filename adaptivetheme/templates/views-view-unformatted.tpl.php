<?php // $Id$
// adaptivethemes.com

/**
 * @file views-view-unformatted.tpl.php
 * Default simple view template to display a list of rows.
 *
 * @ingroup views_templates
 */
?>
<?php
// Conditionally add CSS classes.
$classes = array();
if (theme_get_setting(cleanup_views_unformatted_classes)) {
  $classes = ' ' . $classes[$id];
}
?>
<?php if (!empty($title)): ?>
  <h3><?php print $title; ?></h3>
<?php endif; ?>
<?php foreach ($rows as $id => $row): ?>
  <div class="views-row-item<?php print $classes ? classes : ''; ?>">
    <?php print $row; ?>
  </div>
<?php endforeach; ?> <!-- /views-view-unformatted -->
