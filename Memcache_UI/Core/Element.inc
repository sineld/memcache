<?php
namespace Memcache_UI\Core {

  /**
   * A wrapper for XML elements.
   */
  class Element {
    public $attributes = array();
    public $name = NULL;
    public $new_line; // Add a new line after element
    public $value = NULL;

    public function __construct($name, $attr = NULL, $value = NULL) {
      $this->name = $name;
      $this->attributes = $attr;
      $this->value = $value;
    }

    /**
     * @link drupal7/includes/common.inc#check_plain()
     */
    public static function check_plain($text) {
      return htmlspecialchars($text, ENT_QUOTES, 'UTF-8');
    }

    /**
     * @link drupal7/includes/common.inc#drupal_attributes()
     */
    public static function getSerializedAttributes(array $attributes = array()) {
      foreach ($attributes as $attribute => &$data) {
        $data = implode(' ', (array) $data);
        $data = $attribute . '="' . self::check_plain($data) . '"';
      }

    return $attributes ? ' ' . implode(' ', $attributes) : '';
    }

    public function __toString() {
      $ret = '<'. $this->name;
      if (!empty($this->attributes) && is_array($this->attributes)) {
        $ret .= self::getSerializedAttributes($this->attributes);
      }
      if (empty($this->value)) {
        $ret .= ' />';
      }
      else {
        $ret .= '>';
        if ($this->value instanceof Element) {
          $ret .= (string) $this->value; // force __toString()
        }
        elseif (is_array($this->value)) {
          $ret .= implode("\n", $this->value);
        }
        else {
          $ret .= $this->value;
        }
        $ret .= "</$this->name>";
      }
      return $ret;
    }
  }
}
