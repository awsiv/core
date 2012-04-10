<div style="margin-left: 12px;">
    <h2>References to "<?php echo $topicDetail['topic']; ?>" in the context of "<?php echo $topicDetail['context']; ?>"</h2>
    <?php if (isset($topicHits) && !empty($topicHits)) { ?>
        <table class="bundlelist-table">
            <tr>
                <th scope="col">Context</th>
                <th scope="col">Results</th>
            </tr>
            <?php foreach ((array) $topicHits as $topic) { ?>
                <tr>
                    <td>
                        <?php echo $topic['context']; ?>
                    </td>
                    <td>
                        <?php
                          $target = '';
                        if (strtolower($topic['represents']) == 'description' || $topic['type'] === 4) {
                            echo sprintf("%s (%s) ", $topic['ref'], $topic['represents']);
                        } else {
                            $strPattern = "/^(http|https|ftp):\/\/([A-Z0-9][A-Z0-9_-]*(?:\.[A-Z0-9][A-Z0-9_-]*)+):?(\d+)?\/?/i";
                            $appendPath = rtrim(base_url(),'/'); // remove the trailing slash
                            if (preg_match($strPattern, $topic['ref'])) {
                                $appendPath = ''; // no need to append if its full web url
                                $target = '_blank';
                            }
                          
                            if (preg_match('/^\/docs/', $topic['ref']) ||preg_match('/^\/showexample/', $topic['ref']) ) {
                                $target = '_blank'; // no need to append if its full web url
                            }
                            echo sprintf("<a href='%s' target='%s'>%s</a> (URL)", $appendPath.$topic['ref'],$target, $topic['represents']);
                        }
                        ?>
                    </td>
                </tr>
            <?php } ?>
        </table> 
    <?php } else { ?>
        <div>No information found on this topic.</div>
    <?php } ?>
</div>