<?php
/**
 * Memcache User Interface. Main file.
 *
 * @author Frederic G. MARAND <fgm@osinet.fr>
 *
 * @copyright (c) 2011 Frederic G. MARAND
 *
 * Requirements:
 * - PHP >= 5.3
 *
 * Recommended:
 * - intl extension
 *   - on Ubuntu 11.04, this implies removing libidn, libsimplepie, and
 *     dokuwiki, among others
 * - tidy extension
 */
namespace Memcache_UI {

  /**
   * Wrapper around php tidy class.
   *
   * @param string $html
   *
   * @return void
   */
  function applyTidy (&$html) {
    $config = array(
        'indent'          => TRUE,
        'output-xhtml'    => TRUE,
        'sort-attributes' => 'alpha',
        'wrap'            => 200,
    );
    $tidy = new \tidy();
    $tidy->parseString($html, $config, 'utf8');
    $tidy->cleanRepair();
    $html = (string) $tidy;
  }

  function main() {
    try {
      ob_start();
      //echo '<pre>';

      // Set-up autoloader: it cannot autoload itself.
      $package = 'Memcache_UI';
      require "$package/Core/Autoloader.inc";
      $classLoader = new \SplClassLoader($package, dirname(__FILE__));
      $classLoader->setFileExtension('.inc');
      $classLoader->register();

      // Set up the context
      $context = new Core\Context();
      $context->setMessage("Dirname: [". $context->getBase() . "]", LOG_DEBUG);
      $context->setMessage("Path: [". $context->getPath() . "]", LOG_DEBUG);

      // Obtain the routing information
      $router = new Core\Router($context);
      $item = $router->getRoute();

      $page = new $item['page class']($context, $item);
      $page->emitHeaders();
      echo $page;

      $html = ob_get_clean();

      // Filter it on output
      if ($context->getTidy()) {
        applyTidy($html);
      }
      echo $html;
    }
    catch (Exception $e) {
      echo '<pre>';
      echo $e->getMessage() . PHP_EOL;
      echo $e->getTraceAsString();
      echo "</pre>";
    }
  }

main();
}
