<?php
global $user;

$is_editor = is_user_editor($user);
$show_id = $nid;

$entity_id = NULL;
if ($node = menu_get_object()) {
  $entity_id = $node->nid;
}

$is_member = $is_editor;
$pool_locked = !$is_editor;

$pool_node = node_load($entity_id);

if ($pool_node) {
  $pool_locked = boolval($pool_node->field_closed['und'][0]['value']);
  $is_member = og_is_member('node', $entity_id);
}
else {
  $entity_id = $show_id;
}

$redirect_url = drupal_get_path_alias(current_path());
$show_expired = isset($_SESSION['show_expired'])
  ? $_SESSION['show_expired']
  : FALSE;
?>
<<?php print $ds_content_wrapper;
print $layout_attributes; ?> class="ds-1col <?php print $classes; ?> clearfix">

<?php if (isset($title_suffix['contextual_links'])): ?>
  <?php print render($title_suffix['contextual_links']); ?>
<?php endif; ?>

<form action="<?php print "/my_picks/{$entity_id}"; ?>" method="POST"
      class="clearfix">
  <?php print $ds_content; ?>
  <input type="hidden" name="redirectUrl"
         value="<?php print $redirect_url; ?>"/>
  <?php if (is_user_editor($user) || !$show_expired): ?>
    <input type="hidden" name="show_id" value="<?php print $show_id; ?>"/>
    <input type="submit" value="<?php print t('Save Picks'); ?>"/>
  <?php endif; ?>
  <?php unset($_SESSION['show_expired']); ?>
</form>




<?php if (grunfes_check_mitb($show_id, $enabled)): ?>
  <?php
  $mitb_pick = grunfes_mitb_get_user_pick($show_id, $user->uid);
  $mitb_can_pick = grunfes_mitb_user_can_pick($show_id, $user->uid);
  ?>
    <div id="mitb">
        <form action="<?php echo "/mitb/{$pool_node->nid}/${show_id}/cash_in"; ?>"
              method="POST">
            <div class="views-row left w100">
                <div class="clearfix w100 m5-0">
                    <h3><?php echo t('Money in the bank'); ?></h3>
                    <div class="description">Check the Box if you think the MITB will be Cashed-In. 20 Points if you are Correct! -10 Points if you are Wrong! </div>
                  <?php if ($enabled['mitb_mens']): ?>
                      <div class="p5 mitb_row">
                          <input id="mitb_mens" name="mitb_mens"
                                 type="checkbox" <?php if ($pool_locked) { print 'disabled'; }; ?>
                            <?php if (!$mitb_can_pick) {
                              echo '';
                            } ?>
                            <?php if ($mitb_pick !== NULL && $mitb_pick->field_mitb_mens['und'][0]['value']) {
                              echo 'checked';
                            } ?> />
                          <label for="mitb_mens">Men's</label>
                      </div>
                  <?php endif; ?>

                  <?php if ($enabled['mitb_womens']): ?>
                      <div class="p5 mitb_row">
                          <input id="mitb_womens" name="mitb_womens"
                                 type="checkbox" <?php if ($pool_locked) { print 'disabled'; }; ?>
                            <?php if (!$mitb_can_pick) {
                              echo '';
                            } ?>
                            <?php if ($mitb_pick !== NULL && $mitb_pick->field_mitb_womens['und'][0]['value']) {
                              echo 'checked';
                            } ?> />
                          <label for="mitb_womens">Women's</label>
                      </div>
                  <?php endif; ?>

                  <?php if ($is_member && $mitb_can_pick): ?>
                      <input type="hidden" name="redirectUrl"
                             value="<?php echo $redirect_url; ?>"/>
                      <input type="submit" value="<?php echo t('Save your option'); ?>"/>
                  <?php elseif ($is_member): ?>
                      <div>You have selected MITB. If you want you can change your option until lockup.</div>
                      <input type="submit" value="<?php echo t('Save your option'); ?>"/>
                  <?php endif; ?>
                </div>
            </div>
        </form>
    </div>
<?php endif; ?>




<?php if ($is_editor && can_show_be_published($show_id)): ?>
  <form action="<?php print "/publish_show/{$show_id}"; ?>" method="POST">
    <input type="hidden" name="redirectUrl"
           value="<?php print $redirect_url; ?>"/>
    <input type="submit" value="Publish Show Results"/>
  </form>
<?php endif; ?>

</<?php print $ds_content_wrapper ?>>

<?php if (!empty($drupal_render_children)): ?>
  <?php print $drupal_render_children ?>
<?php endif; ?>
