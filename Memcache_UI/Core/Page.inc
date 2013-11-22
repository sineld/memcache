<?php
namespace Memcache_UI\Core {

  abstract class Page extends Element {

    /**
     * The HTML body element of the page.
     *
     * @var array
     */
    protected $body;

    /**
     * The MIME content type with charset
     *
     * @var string
     */
    protected $contentType = 'text/html; charset=utf8';

    /**
     * @var Context
     */
    protected $context;

    /**
     * The HTML head element of the page.
     *
     * @var array
     */
    protected $head;

    /**
     * The array of HTTP headers
     *
     * @var array
     */
    protected $headers = array();

    /**
     * @var array
     */
    protected $finalized = array();

    /**
     * @var array
     */
    protected $styles;

    /**
     * Page constructor.
     *
     * @param Context $context
     * @param array $item
     *   A router info array.
     *
     * @see Router::getInfo()
     */
    public function __construct(Context $context, array $item) {
      parent::__construct('html');
      $context->setMessage($item, LOG_DEBUG);
      $this->context = $context;
      $this->config = new Config($context);
      $this->initializeHead();
      $this->initializeBody();
      // Will fail if not reimplemented as a concrete method in a child class
      if (method_exists($this, 'build')) {
        $this->build();
      }
    }

    public function __toString() {
      $this->finalizeHeader();
      $html = new Element('html', NULL, $this->getHead() . $this->getBody());
      return (string) $html;
    }

    //public abstract function build();

    public function emitHeaders() {
      $this->finalizeHeader();
      foreach ($this->headers as $name => $value) {
        header("$name: $value");
      }
    }

    public function finalizeBody() {
      if (isset($this->finalized['body'])) {
        throw new Exception('Attempt to finalize already finalized body');
      }
      if ($message = $this->context->getMessage(TRUE)) {
        $items = array();
        foreach ($message as $row) {
          $items[] = new Element('li', array(
            'class' => array($this->context->getLogLevelClass($row[1])),
          ), $row[0]);
        }
        $this->setBody(new Element('div', array('class' => array('messages')), $items), 'messages');
      }
      foreach ($this->getRegions() as $region) {
        $this->body[$region] = implode('', $this->body[$region]);
      }
      $this->finalized['body'] = TRUE;
    }

    public function finalizeHead() {
      if (isset($this->finalized['head'])) {
        throw new Exception('Attempt to finalize already finalized head');
      }
      $cssLink = new Element('link', array(
        'rel' => 'stylesheet',
        'type' => 'text/css',
        'href' => $this->context->getBase() .'/memcache_ui.css',
      ));
      $this->setHead($cssLink);
      $this->finalized['head'] = TRUE;
    }

    public function finalizeHeader() {
      $contentType = $this->getHeader('content-type');
      if (empty($contentType)) {
        $this->setHeader('content-type', $this->contentType);
      }
    }

    public function getBody() {
      $this->finalizeBody();
      $body = new Element('body', NULL, $this->body);
      $ret = (string) $body;
      return $ret;
    }

    public function getHead() {
      $this->finalizeHead();
      $head = new Element('head', NULL, $this->head);
      $ret = (string) $head;
      return $ret;
    }

    public function getHeader($name) {
    }

    public function getRegions() {
      $ret = array(
        'header',
        'first sidebar',
        'content top',
        'messages',
        'help',
        'content',
        'content bottom',
        'second sidebar',
        'footer',
      );

      return $ret;
    }

    public function initializeBody() {
      foreach ($this->getRegions() as $region) {
        $this->body[$region] = array();
      }
    }

    public function initializeHead() {
      $this->setHead(new Element('title', NULL, 'Memcache info'));
    }

    public function setBody($fragment, $region = 'content') {
      if (!in_array($region, $this->getRegions())) {
        $this->context->setMessage(strtr('Attempted to insert data in nonexistent region @region', array(
          '@region' => self::check_plain($region),
        )), LOG_WARNING);
      }
      $this->body[$region][] = $fragment;
    }

    public function setHead($item) {
      $this->head[] = $item;
    }

    public function setHeader($name, $value) {
      $this->headers[$name] = $value;
    }

  }
}
