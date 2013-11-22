<?php
namespace Memcache_UI\Page {

  use Memcache_UI\Core\Element;

  class SlabOverview extends Base {
    public function build() {
      parent::build();
      $this->setBody(new Element('p', NULL, 'Slab overview'));
    }
  }
}
