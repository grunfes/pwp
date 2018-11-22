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

if ($node = menu_get_object()) {
  // Get the nid
  $pool_id = $node->nid;
}

$group_id = $row->_field_data['node_field_data_field_matches_to_pick_nid']['entity']->vid;
?>
<h3 class="left w100"><?php print $fields['title']->content; ?></h3>
<div class="left clearfix w100"><label>Start
    date: </label><?php print $fields['field_start_date']->content; ?></div>
<div class="left clearfix w100"><label>Points to
    earn: </label><?php print $fields['field_point_to_earn']->content; ?></div>

<div class="match clearfix w100 m5-0">
  <?php
  print grunfes_render_matches($pool_id, $fields);
  ?>
</div>

<?php if (isset($fields['views_conditional'])): ?>
  <div class="left clearfix w100">
    <label></label><?php print $fields['views_conditional']->content; ?></div>
<?php endif; ?>
<div class="left clearfix w100"><a href="/editors-picks">Editor's Picks</a>
</div>