<?php
namespace Memcache_UI\Page {

  use Memcache_UI\Core\Element;
  use Memcache_UI\Core\Context;

  class Main extends Base {
    public function build() {
      parent::build();
      $this->setBody(new Element('p', NULL, Context::t("Hello world\n")));
    }
  }
}
