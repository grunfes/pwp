<?php

/**
 * @file
 * Default simple view template to all the fields as a row.
 *
 * - $view: The view in use.
 * - $fields: an array of $field objects. Each one contains:
 *   - $field->content: The output of the field.
 *   - $field->raw: The raw data for the field, if it exists. This is NOT
 *   output safe.
 *   - $field->class: The safe class id to use.
 *   - $field->handler: The Views field handler object controlling this field.
 *   Do not use var_export to dump this object, as it can't handle the
 *   recursion.
 *   - $field->inline: Whether or not the field should be inline.
 *   - $field->inline_html: either div or span based on the above flag.
 *   - $field->wrapper_prefix: A complete wrapper containing the inline_html to
 *   use.
 *   - $field->wrapper_suffix: The closing tag for the wrapper.
 *   - $field->separator: an optional separator that may appear before a field.
 *   - $field->label: The wrap label text to use.
 *   - $field->label_html: The full HTML of the label to use including
 *     configured element type.
 * - $row: The raw result object from the query, with all data it fetched.
 *
 * @ingroup views_templates
 */

$match_id = intval($fields['field_match_id']->raw);
$pools = grunfes_get_pools_from_match($match_id);
?>

<?php if (!empty($pools)): ?>
  <h3 class="left w100"><?php print $fields['title']->content; ?></h3>
  <form action="/picks-administration/<?php print $match_id; ?>/save"
        method="post">

    <div class="match clearfix w100 m5-0">
      <?php
      print grunfes_render_matches($pools[0], $fields);
      ?>
    </div>

    <input type="hidden" name="pool_id" value="<?php print $pools[0]; ?>" />
    <input type="hidden" name="redirectUrl" value="picks-administration"/>
    <input type="submit" value="Save"/>
  </form>
<?php endif; ?>