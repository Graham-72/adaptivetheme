<?php // $Id$
// adaptivethemes.com

/**
 * @file
 * Default theme implementation for displaying a single search result.
 *
 * This template renders a single search result and is collected into
 * search-results.tpl.php. This and the parent template are
 * dependent to one another sharing the markup for definition lists.
 *
 * Available variables:
 * - $url: URL of the result.
 * - $title: Title of the result.
 * - $snippet: A small preview of the result. Does not apply to user searches.
 * - $info: String of all the meta information ready for print. Does not apply
 *   to user searches.
 * - $info_split: Contains same data as $info, split into a keyed array.
 * - $type: The type of search, e.g., "node" or "user".
 *
 * Default keys within $info_split:
 * - $info_split['type']: Node type.
 * - $info_split['user']: Author of the node linked to users profile. Depends
 *   on permission.
 * - $info_split['date']: Last update of the node. Short formatted.
 * - $info_split['comment']: Number of comments output as "% comments", %
 *   being the count. Depends on comment.module.
 * - $info_split['upload']: Number of attachments output as "% attachments", %
 *   being the count. Depends on upload.module.
 *
 * Since $info_split is keyed, a direct print of the item is possible.
 * This array does not apply to user searches so it is recommended to check
 * for their existence before printing. The default keys of 'type', 'user' and
 * 'date' always exist for node searches. Modules may provide other data.
 *
 *   <?php if (isset($info_split['comment'])) : ?>
 *     <span class="info-comment">
 *       <?php print $info_split['comment']; ?>
 *     </span>
 *   <?php endif; ?>
 *
 * To check for all available data within $info_split, use the code below.
 *
 *   <?php print '<pre>'. check_plain(print_r($info_split, 1)) .'</pre>'; ?>
 *
 * @see template_preprocess_search_result()
 */
?>
<li>
  <h3 class="title"><a href="<?php print $url; ?>"><?php print $title; ?></a></h3>
  <?php if ($snippet or $info_split): ?>
    <div class="search-snippet-info">
      <?php if ($snippet) : ?>
        <p class="search-snippet"><?php print $snippet; ?></p>
      <?php endif; ?>

      <?php if ($info_split) : ?>
        <p class="search-info">
          <?php $separator = ''; ?>

          <?php if (isset($info_split['type'])) : ?>
            <span class="search-info-type"><?php print $info_split['type']; ?></span>
            <?php $separator = $info_separator; ?>
          <?php endif; ?>

          <?php if (isset($info_split['user'])) : ?>
            <span class="search-info-user"><?php print $separator . $info_split['user']; ?></span>
            <?php $separator = $info_separator; ?>
          <?php endif; ?>

          <?php if (isset($info_split['date'])) : ?>
            <span class="search-info-date"><?php print $separator . $info_split['date']; ?></span>
            <?php $separator = $info_separator; ?>
          <?php endif; ?>

          <?php if (isset($info_split['comment'])) : ?>
            <span class="search-info-comment"><?php print $separator . $info_split['comment']; ?></span>
            <?php $separator = $info_separator; ?>
          <?php endif; ?>

          <?php if (isset($info_split['upload'])) : ?>
            <span class="search-info-upload"><?php print $separator . $info_split['upload']; ?></span>
          <?php endif; ?>
        </p>
      <?php endif; ?>
    </div>
  <?php endif; ?>
</li>
