<?php

namespace Drupal\at_core\File;

class FileOperations implements FileOperationsInterface {

  /**
   * {@inheritdoc}
   */
  public function fileRename($old_file, $new_file) {
    if (file_exists($old_file)) {
      rename($old_file, $new_file);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function fileStrReplace($file_path, $find, $replace) {
    if (file_exists($file_path)) {
      $file_contents = file_get_contents($file_path);
      $file_contents = str_replace($find, $replace, $file_contents);
      file_put_contents($file_path, $file_contents);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function fileCopyRename($file_paths) {
    if (file_exists($file_paths['copy_source'])) {
      copy($file_paths['copy_source'], $file_paths['copy_dest']);
    }
    if (file_exists($file_paths['rename_oldname'])) {
      rename($file_paths['rename_oldname'], $file_paths['rename_newname']);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function fileBuildInfoYml(array $data, $prefix = NULL) {
    $info = '';
    foreach ($data as $key => $value) {
      if (is_array($value)) {
        $info .= $prefix . "$key:\n";
        $info .= self::fileBuildInfoYml($value, (!$prefix ? '  ' : $prefix = '  '));
      }
      else {
        $info .= is_int($key) ? $prefix . '  - ' . "$value\n" : $prefix . $key . ": $value\n";
      }
    }
    return $info;
  }

}
