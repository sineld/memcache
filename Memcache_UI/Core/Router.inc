<?php
namespace Memcache_UI\Core {
  class Router {
    function __construct(Context $context) {
      $this->context = $context;
    }

    function getInfo() {
      $ret = array(
        '^server/(\w+)/flush/(\w+)$' => array(
          'page class' => 'ServerFlush',
          'page arguments' => array('$1', '$2'),
          'title callback' => 'title_server',
          'title arguments' => 'page arguments',
        ),
        '^server/(\w+)/flush$' => array(
          'page class' => 'ServerFlush',
          'page arguments' => array('$1'),
          'title callback' => 'title_server',
          'title arguments' => 'page arguments',
        ),

        '^server/(\w+)/slab/(\d+)$' => array(
          'page class' => 'SlabView',
          'page arguments' => array('$1', '$2'),
          'title callback' => 'title_slab',
          'title arguments' => 'page arguments',
        ),

        '^server/(\w+)/key/(.+)/delete/(\w+)$' => array(
          'page class' => 'VariableDeleteConfirm',
          'page arguments' => array('$1', '$2', '$3'),
          'title callback' => 'title_variable',
          'title arguments' => 'page arguments',
        ),
        '^server/(\w+)/key/(.+)/delete$' => array(
          'page class' => 'VariableDeleteForm',
          'page arguments' => array('$1', '$2'),
          'title callback' => 'title_variable',
          'title arguments' => 'page arguments',
        ),

        '^server/(\w+)/key/(.+)/dump$' => array(
          'page class' => 'VariableViewPhp',
          'page arguments' => array('$1', '$2'),
          'title callback' => 'title_variable',
          'title arguments' => 'page arguments',
        ),
        '^server/(\w+)/key/(.+)/php$' => array(
          'page class' => 'VariableViewPhp',
          'page arguments' => array('$1', '$2'),
          'title callback' => 'title_variable',
          'title arguments' => 'page arguments',
        ),
        '^server/(\w+)/key/(.+)$' => array(
          'page class' => 'VariableViewText',
          'page arguments' => array('$1', '$2'),
          'title callback' => 'title_variable',
          'title arguments' => 'page arguments',
        ),

        '^slabs$' => array(
          'page class' => 'SlabOverview',
          'title' => 'Slabs per server',
        ),

        '' => array( // Catch-all regex
          'page class' => 'Main',
          'title' => 'Overview',
        ),
      );

      return $ret;
    }

    function getRoute() {
      $found = FALSE;
      $path = $this->context->getPath();
      $matches = array();
      $infoDefaults = array(
        'page arguments' => array(),
      );
      foreach ($this->getInfo() as $regex => $info) {
        $regex = "@$regex@";
        $count = preg_match($regex, $path, $matches);
        if ($count) {
          $found = TRUE;
          break;
        }
      }
      if ($found) {
        $this->context->setMessage("Menu hit on $regex", LOG_DEBUG);
        $info = array_merge($infoDefaults, $info);
        $this->context->setMessage("Info: ". print_r($info, TRUE), LOG_DEBUG);
        $info['page class'] = 'Memcache_UI\Page\\' . $info['page class'];
        if (!empty($info['page arguments'])) {
          $regexes = array_fill(0, count($info['page arguments']), $regex);
          $paths = array_fill(0, count($info['page arguments']), $path);
          $info['page arguments'] = preg_replace($regexes, $info['page arguments'], $paths);
        }
        if (isset($info['title arguments']) && $info['title arguments'] == 'page arguments') {
          $info['title arguments'] = &$info['page arguments'];
        }
        $this->context->setMessage("Info resolved: ". print_r($info, TRUE), LOG_DEBUG);
      }
      else {
        $info = NULL;
      }

      return $info;
    }
  }
}
