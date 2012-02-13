<?php
foreach ((array) $viewdata as $index => $data) {  ?><li><span class="type"><?php echo $data[3] ?></span><p><?php echo sprintf('<a href="/promise/details/%s" class="promiselnk" title="promise"><span class="promiser">%s</span></a>', urlencode($data[0]), $data[4]);
    ?><?php if (!$showOnlyHandle) echo sprintf('<a class="handleClick" href="/promise/details/%s" rel="%s" title="handle"><span class="handle">%s</span></a>', $data[0], $data[0], $data[0]);
    ?><?php if (!$showOnlyHandle) echo sprintf('<a href="/bundle/details/bundle/%s/type/%s" title="bundle"><span class="bundle">%s</span></a>', $data[2], $data[3], $data[2]);
    ?></p></li>
<?php } ?>