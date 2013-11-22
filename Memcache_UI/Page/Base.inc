<?php

namespace Memcache_UI\Page {

  use Memcache_UI\Core\Element;

  /**
   * Provide common layout elements to child classes.
   *
   * - header
   */
  class Base extends \Memcache_UI\Core\Page {
    function build() {
      $this->setBody(
        new Element('div', array('class' => array('head')),
          new Element('h1', array('class' => array('memcache')), array(
            new Element('span', array('class' => array('logo')),
              new Element('a', array('href' => 'http://pecl.php.net/package/memcache'), 'memcache')
            ),
            new Element('span', array('class' => array('nameinfo')), 'memcache_ui.php from an idea by <a href="http://livebookmark.net">Harun Yayli</a>'),
          ))),
        'header'
      );
    }
  }
}