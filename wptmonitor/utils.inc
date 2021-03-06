<?php
/**
 * Get a array of folders that are shared with the $userId
 *
 * @param  $userId
 * @param  $table
 * @return array
 */
function getFolderShares($userId, $table, $ownerId = null, $permission = -1) {
  $folderTable = Doctrine_Core::getTable($table . 'Folder');

  $q = Doctrine_Query::create()->from('Share s')
      ->where('ShareWithUserId = ?', $userId);
  if ($ownerId != null) {
    $q->andWhere('UserId = ?', $ownerId);
  } else {
    $q->andWhere('UserId != ?', $userId);
  }
  $q->andWhere('TheTableName = ?', $table)
      ->andWhere('Permissions > ?', $permission)
      ->orderBy('UserId')
      ->setHydrationMode(Doctrine_Core::HYDRATE_ARRAY);

  $shares = $q->fetchArray();
  $shareArray = array();
  $userTable = Doctrine_Core::getTable('User');

  if ($q->count() > 0) {
    foreach ($shares as $share) {
      $user = $userTable->find($share['UserId']);
      $folderItem = $folderTable->find($share['TableItemId']);
      $shareArray[$user['Username']][$folderItem['id']] = array('Label' => $folderItem['Label'], 'Permission' => $share['Permissions']);
    }
  }
  //    print_r($shares);
  //    exit;

  $q->free(true);
  return $shareArray;
}

function getFolderIdFor($id, $tableName) {
  $table = Doctrine_Core::getTable($tableName);
  $res = $table->find($id);
  return $res[$tableName . 'FolderId'];
}

function getOwnerIdFor($id, $table) {
  $table = Doctrine_Core::getTable($table);
  $res = $table->find($id);
  return $res['UserId'];
}

/**
 * Get the folder tree for the indicated user and db table name
 *
 * @param  $userId
 * @param  $table
 * @return
 */
function getFolderTree($userId, $table) {
  $treeObject = Doctrine_Core::getTable($table . 'Folder')->getTree();
  $folderTree = $treeObject->fetchTree(array('root_id' => $userId));
  // Check for root node and add if not there

  if (!$treeObject->fetchRoot($userId)) {
    if ($table == "WPTJob") {
      $folder = new WPTJobFolder();
    } else if ($table == "WPTScript") {
      $folder = new WPTScriptFolder();
    } else if ($table == "Alert") {
      $folder = new AlertFolder();
    } else if ($table == "ChangeNote") {
      $folder = new ChangeNoteFolder();
    }
    $folder->Label = "Root";
    $folder->root_id = $userId;
    $folder->save();
    $treeObject->createRoot($folder, $userId);
    $folderTree = $treeObject->fetchTree(array('root_id' => $userId));
  }

  return $folderTree;
}

function getRootFolderForUser($userId, $table) {
  $treeObject = Doctrine_Core::getTable($table.'Folder')->getTree();
//  $folderTree = $treeObject->fetchTree(array('root_id' => $userId));
  $folderTree = getFolderTree($userId, $table);
  return $folderTree[0]['id'];
}

function displayErrorIfNotAdmin() {
  if ($_SESSION['ls_admin'] != 1) {
    displayError();
  }
}

function displayError($message = "You are not authorized to perform this operation") {
  $smarty = new Smarty;
  $smarty->assign('errorMessage', $message);
  $smarty->display('error.tpl');
  exit;
}

function getCurrentUserId() {
  // Support admin impersonating impersonate
  if (isset($_SESSION['ls_admin'])) {
    if ($_SESSION['ls_impersonate_id']) {
      return $_SESSION['ls_impersonate_id'];
    }
  } else if (isset($_SESSION['ls_guest_id'])) {
    return $_SESSION['ls_guest_id'];
  } else {
    return $_SESSION['ls_id'];
  }
}

function getBaseURL() {
  $serverName = $_SERVER['SERVER_NAME'];
  $lidx = strrpos($_SERVER['REQUEST_URI'], "/");
  $baseurl = $serverName .":".$_SERVER['SERVER_PORT']. substr($_SERVER['REQUEST_URI'], 0, $lidx) . "/";
  return "http://" . $baseurl;
}

// Turns a delimited file into a two dimensional array.
function delimitedToArray($data, $delimiter, $header = false) {
  $rawDataHeaders[]='Date';
  $rawDataHeaders[]='Time';
  $rawDataHeaders[]='Event Name';
  $rawDataHeaders[]='URL';
  $rawDataHeaders[]='Load Time (ms)';
  $rawDataHeaders[]='Time to First Byte (ms)';
  $rawDataHeaders[]='unused';
  $rawDataHeaders[]='Bytes Out';
  $rawDataHeaders[]='Bytes In';
  $rawDataHeaders[]='DNS Lookups';
  $rawDataHeaders[]='Connections';
  $rawDataHeaders[]='Requests';
  $rawDataHeaders[]='OK Responses';
  $rawDataHeaders[]='Redirects';
  $rawDataHeaders[]='Not Modified';
  $rawDataHeaders[]='Not Found';
  $rawDataHeaders[]='Other Responses';
  $rawDataHeaders[]='Error Code';
  $rawDataHeaders[]='Time to Start Render (ms)';
  $rawDataHeaders[]='Segments Transmitted';
  $rawDataHeaders[]='Segments Retransmitted';
  $rawDataHeaders[]='Packet Loss (out)';
  $rawDataHeaders[]='Activity Time(ms)';
  $rawDataHeaders[]='Descriptor';
  $rawDataHeaders[]='Lab ID';
  $rawDataHeaders[]='Dialer ID';
  $rawDataHeaders[]='Connection Type';
  $rawDataHeaders[]='Cached';
  $rawDataHeaders[]='Event URL';
  $rawDataHeaders[]='Pagetest Build';
  $rawDataHeaders[]='Measurement Type';
  $rawDataHeaders[]='Experimental';
  $rawDataHeaders[]='Doc Complete Time (ms)';
  $rawDataHeaders[]='Event GUID';
  $rawDataHeaders[]='Time to DOM Element (ms)';
  $rawDataHeaders[]='Includes Object Data';
  $rawDataHeaders[]='Cache Score';
  $rawDataHeaders[]='Static CDN Score';
  $rawDataHeaders[]='One CDN Score';
  $rawDataHeaders[]='GZIP Score';
  $rawDataHeaders[]='Cookie Score';
  $rawDataHeaders[]='Keep-Alive Score';
  $rawDataHeaders[]='DOCTYPE Score';
  $rawDataHeaders[]='Minify Score';
  $rawDataHeaders[]='Combine Score';
  $rawDataHeaders[]='Bytes Out (Doc)';
  $rawDataHeaders[]='Bytes In (Doc)';
  $rawDataHeaders[]='DNS Lookups (Doc)';
  $rawDataHeaders[]='Connections (Doc)';
  $rawDataHeaders[]='Requests (Doc)';
  $rawDataHeaders[]='OK Responses (Doc)';
  $rawDataHeaders[]='Redirects (Doc)';
  $rawDataHeaders[]='Not Modified (Doc)';
  $rawDataHeaders[]='Not Found (Doc)';
  $rawDataHeaders[]='Other Responses (Doc)';
  $rawDataHeaders[]='Compression Score';
  $rawDataHeaders[]='Host';
  $rawDataHeaders[]='IP Address';
  $rawDataHeaders[]='ETag Score';
  $rawDataHeaders[]='Flagged Requests';
  $rawDataHeaders[]='Flagged Connections';
  $rawDataHeaders[]='Max Simultaneous Flagged Connections';
  $rawDataHeaders[]='Time to Base Page Complete (ms)';
  $rawDataHeaders[]='Base Page Result';
  $rawDataHeaders[]='Gzip Total Bytes';
  $rawDataHeaders[]='Gzip Savings';
  $rawDataHeaders[]='Minify Total Bytes';
  $rawDataHeaders[]='Minify Savings';
  $rawDataHeaders[]='Image Total Bytes';
  $rawDataHeaders[]='Image Savings';
  $rawDataHeaders[]='Base Page Redirects';
  $rawDataHeaders[]='Optimization Checked';
  $rawDataHeaders[]='AFT (ms)';
  $rawDataHeaders[]='DOM Elements';
  $rawDataHeaders[]='Page Speed Version';
  $rawDataHeaders[]='Page Title';
  $rawDataHeaders[]='Time to Title';
  $rawDataHeaders[]='Load Event Start';
  $rawDataHeaders[]='Load Event End';
  $rawDataHeaders[]='DOM Content Ready Start';
  $rawDataHeaders[]='DOM Content Ready End';
  $rawDataHeaders[]='Visually Complete (ms)';
  // Header array for agents that don't provide them
  $results = array();
  $lines = explode(PHP_EOL, $data);
  $headers = array();

  $heads = explode($delimiter, $lines[0]);
  foreach ($heads as $key => $h) {
    $headers[] = trim($h);
  }
/*print_r($heads);
print_r($lines);*/
  foreach ($lines as $key => $line) {
    if (!$header && $key == 0) {
      continue;
    }
    if (empty($line)){
      continue;
    }
    $items = explode($delimiter, $line);
    $idx=0;
    foreach ($items as $key => $item) {
      //$label = $headers[$key];
      $label = $rawDataHeaders[$idx];
      $result[$label] = $item;
      $idx++;
    }
    $results[] = $result;
  }
  return $results;
}

/**
 * Returns an array of items from a given result set.
 *
 * @param  $results
 * @param  $item
 * @return array
 */
function getArrayFor($results, $item) {
  $res = array();
  foreach ($results as $result) {
    $res[] = $result[$item];
  }
  return $res;
}

/**
 * Return the standard deviation
 *
 * @param  $std
 * @return float
 */
function standard_deviation($std) {
  $total = 0;
  while (list($key, $val) = each($std))
  {
    $total += $val;
  }
  reset($std);
  $mean = $total / count($std);

  while (list($key, $val) = each($std))
  {
    $sum += pow(($val - $mean), 2);
  }
  $var = sqrt($sum / (count($std) - 1));
  return $var;
}

function getConfig() {
  $configTable = Doctrine_Core::getTable('WPTMonitorConfig');
  $config = $configTable->find(1);
  return $config;
}

function getLocationThreshold($host, $location) {
  $q = Doctrine_Query::create()->select('l.QueueThreshold')->from('WPTLocation l')
      ->where('l.WPTHost.HostURL = ?', $host)
      ->andWhere('l.Location= ?', $location)
      ->setHydrationMode(Doctrine_Core::HYDRATE_ARRAY);
  $result = $q->fetchOne();
  if (!$queueThreshold = $result['QueueThreshold']) {
    $queueThreshold = 0;
  }
  $q->free(true);
  return $queueThreshold;
}
function getCurrentRunRateInfo(){
  $runRateInfo = array();
  $userTable= Doctrine_Core::getTable('User');
    $q = Doctrine_Query::create()
    ->select('u.Username')
    ->from('User u')
    ->orderBy('u.Username');
  $users = $q->fetchArray();
  foreach($users as $user){
    $runRateInfo['users'][] = $user['Username'];
  }

  $q = Doctrine_Query::create()->select('j.UserId, j.FirstViewOnly, SUM(j.Runs * (2-(FirstViewOnly is not null)) * 60/j.Frequency) as runrate')
      ->from('WPTJob j')
      ->where('j.Active = ?',true)
      ->orderBy('j.UserId')
      ->groupBy('j.UserId');
  $result = $q->fetchArray();
  $totalRunRate = 0;
  $runRatePerUser = array();
  foreach($result as $res){
    $user = $userTable->find($res['UserId']);
//    if ( !$res['FirstViewOnly']){
//      $runRatePerUser[$user['Username']] = $res['runrate']*2;
//    }else {
      $runRatePerUser[$user['Username']] = $res['runrate'];
//    }
    $totalRunRate += $runRatePerUser[$user['Username']];
  }
  $runRateInfo['runRatePerUser'] = $runRatePerUser;
  $runRateInfo['hourlyRunRate'] = $totalRunRate;

  // Active Jobs per User
  $q = Doctrine_Query::create()->select('j.UserId, COUNT(j.UserId) as jobCount')
      ->from('WPTJob j')
      ->where('j.Active = ?',true)
      ->orderBy('j.UserId')
      ->groupBy('j.UserId');
  $result = $q->fetchArray();

  $activeJobsPerUser= array();
  foreach($result as $res){
    $user = $userTable->find($res['UserId']);
    $activeJobsPerUser[$user['Username']] = $res['jobCount'];
  }
  $runRateInfo['activeJobsPerUser'] = $activeJobsPerUser;
  // All jobs per user

  $q = Doctrine_Query::create()->select('j.UserId, COUNT(j.UserId) as jobCount')
      ->from('WPTJob j')
      ->orderBy('j.UserId')
      ->groupBy('j.UserId');
  $result = $q->fetchArray();

  $jobsPerUser= array();
  foreach($result as $res){
    $user = $userTable->find($res['UserId']);
    $jobsPerUser[$user['Username']] = $res['jobCount'];
  }
  $runRateInfo['jobsPerUser'] = $jobsPerUser;

  $q = Doctrine_Query::create()->from('WPTJob j');
  $runRateInfo['totalJobs'] = $q->count();

  $q = Doctrine_Query::create()->from('WPTJob j')
      ->where('j.Active = ?',true);
  $runRateInfo['totalActiveJobs']= $q->count();

  $q->free(true);
  return $runRateInfo;
}
function getTestersInformation($host = null){
  if ($host == null) {
    $q = Doctrine_Query::create()->from('WPTHost h')->setHydrationMode(Doctrine_Core::HYDRATE_ARRAY);
    $hosts = $q->fetchArray();
    $q->free(true);
  } else {
    $q = Doctrine_Query::create()->from('WPTHost h')
        ->where('h.HostURL = ?', $host)->setHydrationMode(Doctrine_Core::HYDRATE_ARRAY);
    $host = $q->fetchOne();
    $hosts[] = $host;
  }
  $testerInformation = array();
  foreach( $hosts as $host){
    $testerInfo = array();
    $resultXml = file_get_contents($host['HostURL'] . "/getTesters.php?f=xml");
    if (!$resultXml) {
      logOutput("[ERROR] [getTestersInformation] Failed to retrieve testers information");
    }
    $xml = new SimpleXMLElement($resultXml);
    
    foreach ($xml->data->location as $loc) {
      $id = $loc->id;
      $cnt = 0;
      $agents = array();
      if ( $loc->testers->tester ){
        foreach ($loc->testers->tester as $tester){
          if ($tester->elapsed < 60){
            $t = array();
            $t['id'] = $id;
            $t['index'] = $tester->index;
            $t['pc'] = $tester->pc;
            $t['ec2'] = $tester->ec2;
            $t['ip'] = $tester->ip;
            $t['elapsed'] = $tester->elapsed;
            $t['last'] = $tester->last;
            $t['busy'] = $tester->busy;
              $cnt++;
            $agents[] = $t;
          }
        }
      }
      $testerInfo['AgentCount']= $cnt;
      $testerInfo['XML'] = $loc;
      $testerInfo['Agents'] = $agents;
      $testerInfo['id'] = $id;
      $testerInformation['"'.$id.'"'] = $testerInfo;
    }
  }

  return $testerInformation;
}

function getLocationInformation($host = null) {
  $testerInformation = getTestersInformation($host);
  if ($host == null) {
    $q = Doctrine_Query::create()->from('WPTHost h')->setHydrationMode(Doctrine_Core::HYDRATE_ARRAY);
    $hosts = $q->fetchArray();
    $q->free(true);
  } else {
    $q = Doctrine_Query::create()->from('WPTHost h')
        ->where('h.HostURL = ?', $host)->setHydrationMode(Doctrine_Core::HYDRATE_ARRAY);
    $host = $q->fetchOne();
    $hosts[] = $host;
  }

  $locationTable = Doctrine_Core::getTable('WPTLocation');

  $locations = array();
  foreach ($hosts as $host) {
    $location = array();
    $resultXml = file_get_contents($host['HostURL'] . "/getLocations.php?f=xml");
    if (!$resultXml) {
      continue;
    }
    $xml = new SimpleXMLElement($resultXml);
    foreach ($xml->data->location as $loc) {
      $location['host'] = $host['Label'];
      $location['hostURL'] = $host['HostURL'];
      $location['id'] = $loc->id;
      $id = $loc->id;
      $location['Label'] = $loc->Label;
      $location['Browser'] = $loc->Browser;
      $location['PendingTests'] = $loc->PendingTests->Total;
      $location['PendingTestsHighPriority'] = $loc->PendingTests->HighPriority;
      $location['PendingTestsLowPriority'] = $loc->PendingTests->LowPriority;
      $location['AgentCount'] = $testerInformation['"'.$id.'"']['AgentCount'];
      $loc = $locationTable->findOneByLocation($loc->id);
      if (!$location['GreenLimit'] = $loc['QueueThresholdGreenLimit']) {
        $location['GreenLimit'] = 15;
      }
      if (!$location['YellowLimit'] = $loc['QueueThresholdYellowLimit']) {
        $location['YellowLimit'] = 30;
      }
      $locations["'".$id."'"] = $location;
    }
  }
  //SELECT location, firstviewonly, runs, frequency, (runs * (2-(firstviewonly is not null))*60/frequency) as runrate FROM `wptjob` WHERE active group by location;
  $q = Doctrine_Query::create()->select('j.Location, j.FirstViewOnly, SUM(j.Runs * (2-(FirstViewOnly is not null)) * 60/j.Frequency) as runrate')
      ->from('WPTJob j')
      ->where('j.Active = ?',true)
      ->orderBy('j.Location')
      ->groupBy('j.Location');

  $result = $q->fetchArray();

  foreach ($result as $res){
    $locations["'".$res['Location']."'"]['runRate'] = $res['runrate'];
  }


  return $locations;
}

function logOutput($msg, $file="jobProcessor_log.html") {
  //$timeStamp = date(DATE_W3C);
	$timeStamp = date("m/d/Y h:i:s");
  $a = "";
  $m = "[ " . $timeStamp . " ] " . $msg . "\n";
  if (!($fp = fopen($file, 'a'))) {
    echo "[ERROR] [logOutput] Cannot open log file";
  } else {
    fwrite($fp, $a . "" . $m);
  }

  // truncate only once per 
  static $already_truncated = false;

  if($already_truncated === false) {
    ftruncate($fp, 100000); // 100Kb max
    $already_truncated = true;
  }
    
  fclose($fp);
}

function get_tz_options($selectedzone, $name = "edited_user_timezone", $label = '', $desc = '', $onChange = '') {
  //  echo '<div class="label"><label for="edited_user_timezone">'.$label.':</label></div>';
  //  echo '<div class="input"><select name="edited_user_timezone" onchange="'.$onChange.'">';
  $field = '<select name="' . $name . '" onchange="' . $onChange . '">';
  $field .= timezonechoice($selectedzone);
  $field .= '</select>';
  return $field;
  //  echo '<span class="notes"> '.$desc.' </span></div>';
}

function timezonechoice($selectedzone) {
  $all = timezone_identifiers_list();

  $i = 0;
  foreach ($all AS $zone) {
    $zone = explode('/', $zone);
    $zonen[$i]['continent'] = isset($zone[0]) ? $zone[0] : '';
    $zonen[$i]['city'] = isset($zone[1]) ? $zone[1] : '';
    $zonen[$i]['subcity'] = isset($zone[2]) ? $zone[2] : '';
    $i++;
  }

  asort($zonen);
  $structure = '';
  $structure .= "<option " . ((('UTC') == $selectedzone) ? 'selected="selected "' : '') . " value=\"" . ('UTC') . "\">GMT</option>"; //Timezone

  foreach ($zonen AS $zone) {
    extract($zone);
    if ($continent == 'Africa' || $continent == 'America' || $continent == 'Antarctica' || $continent == 'Arctic' || $continent == 'Asia' || $continent == 'Atlantic' || $continent == 'Australia' || $continent == 'Europe' || $continent == 'Indian' || $continent == 'Pacific') {
      if (!isset($selectcontinent)) {
        $structure .= '<optgroup label="' . $continent . '">'; // continent
      } elseif ($selectcontinent != $continent) {
        $structure .= '</optgroup><optgroup label="' . $continent . '">'; // continent
      }

      if (isset($city) != '') {
        if (!empty($subcity) != '') {
          $city = $city . '/' . $subcity;
        }
        $structure .= "<option " . ((($continent . '/' . $city) == $selectedzone) ? 'selected="selected "' : '') . " value=\"" . ($continent . '/' . $city) . "\">" . str_replace('_', ' ', $city) . "</option>"; //Timezone
      } else {
        if (!empty($subcity) != '') {
          $city = $city . '/' . $subcity;
        }
        $structure .= "<option " . (($continent == $selectedzone) ? 'selected="selected "' : '') . " value=\"" . $continent . "\">" . $continent . "</option>"; //Timezone
      }

      $selectcontinent = $continent;
    }
  }
  $structure .= '</optgroup>';
  return $structure;
}

function sendEmailReport($emailAddresses, $message, $attachments) {
  include_once 'bootstrap.php';
  $configTable = Doctrine_Core::getTable('WPTMonitorConfig');
  $config = $configTable->find(1);

  // Create a boundary string.  It needs to be unique
  $semi_rand = md5(time());

  $from = $config['SiteName'];
  $fromEmail = 'noreply@' . $config['SiteName'];
  $fromEmail = 'tonyperkins@yahoo.com';
  $siteName = $config['SiteName'];
  if (strrpos($emailAddresses, ",") > 0) {
    $addrs = explode(",", $emailAddresses);
  } else {
    $addrs = explode("\n", $emailAddresses);
  }
  $success = 1;
  foreach ($addrs as $emailAddress) {
    $to = $emailAddress;
    $subject = $siteName . " - WPTMonitor Report";
    $headers = 'From: ' . $from . ' <' . $fromEmail . ">\r\n" .
        'Reply-To: ' . $fromEmail . "\r\n" .
        'Content-type: text/plain; charset=UTF-8' . "\r\n" .
        'X-Mailer: PHP/' . phpversion();

    $mime_boundary = "==Multipart_Boundary_x{$semi_rand}x";
    // Add the headers for a file attachment
    $headers .= "\nMIME-Version: 1.0\n" .
        "Content-Type: multipart/alternative;\n" .
        " boundary=\"{$mime_boundary}\"";
    $message = "--{$mime_boundary}\n" .
        "Content-Type: text/html; charset=\"iso-8859-1\"\n" .
        "Content-Transfer-Encoding: 7bit\n\n" .
        "<font face=Arial>" .
        $message . "\r\n";
    $message .= "--{$mime_boundary}\n" .
        "Content-Type: text/plain; charset=\"iso-8859-1\"\n" .
        "Content-Transfer-Encoding: 7bit\n\n" .
// Add the headers for a file attachment
        $headers .= "\nMIME-Version: 1.0\n" .
            "Content-Type: multipart/mixed;\n" .
            " boundary=\"{$mime_boundary}\"";
    foreach ($attachments as $attach) {
      $fileatt = 'graph/cache/' . $attach; //relative path to image two and more (this one is in the same directory)
      $fileatt_name = $attach; //just the name of the file
      $fileatt_type = filetype($fileatt);
      $file = fopen($fileatt, 'rb');
      $data = fread($file, filesize($fileatt));

      // Base64 encode the file data
      $data = chunk_split(base64_encode($data));
      // Add file attachment to the message
      $message .= "--{$mime_boundary}\n" .
          "Content-Type: image/png;\n" . // {$fileatt_type}
          " name=\"{$fileatt_name}\"\n" .
          "Content-Disposition: inline;\n" .
          " filename=\"{$fileatt_name}\"\n" .
          "Content-Transfer-Encoding: base64\n\n" .
          $data . "\n\n" .
          "--{$mime_boundary}--\n";
    }


    if (mail($to, $subject, $message, $headers)) {
      logOutput("[INFO] [sendEmailAlert] Email Report sent to contact: " . $emailAddress);
      $success = 1;
    } else {
      logOutput("[INFO] [sendEmailAlert] Failed to send alert Report to contact");
      $success = 0;
    }
  }
  return $success;
}

function compressCrypt($string) {
  return base64_encode(gzcompress($string, 9));
}

function decompressCrypt($string) {
  return gzuncompress(base64_decode($string));
}

function getUserIdForFolder($table, $folderId) {
  $folderTable = Doctrine_Core::getTable($table . 'Folder');
  $folder = $folderTable->find($folderId);
  return $folder['root_id'];
}

function getPermissionLevel($table, $folderId){
  $permlevel = -1;
  try {
    // See if user is the owner first
    $userId = getCurrentUserId();

    $folderTable = Doctrine_Core::getTable($table . 'Folder');

    $q = Doctrine_Query::create()
        ->select('f.Label, j.Label')
        ->from($table . 'Folder' . ' f')
        ->leftJoin('f.' . $table . ' j');

    $treeObject = $folderTable->getTree();
    $treeObject->setBaseQuery($q);

    $tree = $treeObject->fetchTree(array('root_id' => $userId));

    $treeObject->resetBaseQuery();
  } catch (Exception $ex){
    error_log("Failed to getPermissionLevel".$ex->getMessage());
  }
    if ($tree) {
      foreach ($tree as $node) {
        if ($node['id'] == $folderId) {
          if ($userId == $node['root_id']) {
            // User owns this folder so return true for all permission checks.
            $permlevel = PERMISSION_OWNER;
          }
        }
      }
    }
    if (!$permlevel){
      $q = Doctrine_Query::create()->from('Share s')
          ->where('s.ShareWithUserId = ?', $userId)
          ->andWhere('s.TheTableName =?', $table)
          ->andWhere('s.TableItemId = ?', $folderId);
      $result = $q->fetchOne();
      $q->free(true);
      $permlevel = $result['Permissions'];
    }
  return $permlevel;
}
// Permission Functions
function hasPermission($table, $folderId, $permissionLevel) {
//  if ( $permissionLevel >= getPermissionLevel($table,$folderId)) {
//    return true;
//  } else {
//    return false;
//  }
  try {
    $status = false;
    // See if user is the owner first
    $userId = getCurrentUserId();

    $folderTable = Doctrine_Core::getTable($table . 'Folder');

    $q = Doctrine_Query::create()
        ->select('f.Label, j.Label')
        ->from($table . 'Folder' . ' f')
        ->leftJoin('f.' . $table . ' j');

    $treeObject = $folderTable->getTree();
    $treeObject->setBaseQuery($q);

    $tree = $treeObject->fetchTree(array('root_id' => $userId));

    $treeObject->resetBaseQuery();

    if ($tree) {
      foreach ($tree as $node) {
        if ($node['id'] == $folderId) {
          if ($userId == $node['root_id']) {
            // User owns this folder so return true for all permission checks.
            $status = true;
          }
        }
      }
    }
    //else {
    if (!$status){
      $q = Doctrine_Query::create()->from('Share s')
          ->where('s.ShareWithUserId = ?', $userId)
          ->andWhere('s.TheTableName =?', $table)
          ->andWhere('s.TableItemId = ?', $folderId);
      $result = $q->fetchOne();
      $q->free(true);
      if ($result['Permissions'] >= $permissionLevel) {
        $status= true;
      } else {
        $status= false;
      }
    }
  } catch (Exception $e) {
    error_log("[WPTMonitor] Failed while Checking permission on Shares. message: " . $e->getMessage());
    echo $e->getMessage();
    exit;
  }
  return $status;
}

function writeIniFile($assoc_arr, $path, $has_sections=FALSE) {
      $content = "";
      if ($has_sections) {
          foreach ($assoc_arr as $key=>$elem) {

              $content .= '["'.str_replace("'", '', $key).'"]'.PHP_EOL;
              foreach ($elem as $key2=>$elem2) {
                  if(is_array($elem2))
                  {
                      for($i=0;$i<count($elem2);$i++)
                      {
                          $content .= $key2."[] = \"".$elem2[$i]."\"".PHP_EOL;
                      }
                  }
                  else if($elem2=="") $content .= $key2." = ".PHP_EOL;
                  else $content .= $key2." = \"".$elem2."\"".PHP_EOL;
              }
          }
      }
      else {
          foreach ($assoc_arr as $key=>$elem) {
              if(is_array($elem))
              {
                  for($i=0;$i<count($elem);$i++)
                  {
                      $content .= $key."[] = \"".$elem[$i]."\"".PHP_EOL;
                  }
              }
              else if($elem=="") $content .= $key." = ".PHP_EOL;
              else $content .= $key." = \"".$elem."\"".PHP_EOL;
          }
      }

      $content .= PHP_EOL.PHP_EOL;

      if (!$handle = fopen($path, 'w')) {
          return false;
      }
      if (!fwrite($handle, $content)) {
          return false;
      }
      fclose($handle);
      return true;
  }

/**
* Update the feeds
*
*/
function UpdateFeeds()
{
    $feedData = array();

    $lockFile = fopen( 'temp/feeds.lock', 'w',  false);
    if( $lockFile )
    {
        // make sure we're not trying to update the same feeds in parallel
        if( flock($lockFile, LOCK_EX | LOCK_NB) )
        {
            // load the list of feeds
            require_once('./settings/feeds.inc');
            require_once('./lib/simplepie.inc');

            // loop through and update each one
            foreach( $feeds as $category => &$feedList )
            {
                $feedData[$category] = array();

                foreach( $feedList as $feedSource => $feedUrl )
                {
                    $feedUrl = trim($feedUrl);
                    if( strlen($feedUrl) )
                    {
                        $feed = new SimplePie();
                        if( $feed )
                        {
                            $ctx = stream_context_create(array(
                                'http' => array(
                                    'timeout' => 10
                                    )
                                )
                            );
                            $rawFeed = file_get_contents($feedUrl,0 , $ctx);
                            $feed->set_raw_data($rawFeed);
                            $feed->enable_cache(false);
                            $feed->init();

                            // try sanitizing the data if we have a problem parsing the feed
                            if( strlen($feed->error()) )
                            {
                                FixFeed($rawFeed);
                                $feed->set_raw_data($rawFeed);
                                $feed->enable_cache(false);
                                $feed->init();
                            }

                            foreach ($feed->get_items() as $item)
                            {
                                $dateStr = $item->get_date(DATE_RSS);
                                if( $dateStr && strlen($dateStr) )
                                {
                                    $date = strtotime($dateStr);
                                    if( $date )
                                    {
                                        // only include articles from the last 30 days
                                        $now = time();
                                        $elapsed = 0;
                                        if( $now > $date )
                                            $elapsed = $now - $date;
                                        $days = (int)($elapsed / 86400);
                                        if( $days <= 30 )
                                        {
                                            // make sure we don't have another article from the exact same time
                                            while(isset($feedData[$category][$date]))
                                                $date++;

                                            $feedData[$category][$date] = array (
                                                    'source' => $feedSource,
                                                    'title' => $item->get_title(),
                                                    'link' => urldecode($item->get_permalink()),
                                                    'date' => $dateStr
                                                );
                                        }
                                    }
                                }

                                $item->__destruct();
                            }

                            $feed->__destruct();
                            unset($feed);
                        }
                    }
                }

                if( count($feedData[$category]) )
                    krsort($feedData[$category]);
            }

            // save out the feed data
            file_put_contents('temp/feeds.dat', json_encode($feedData));
            fclose($lockFile);
        }
    }
}

/**
* MyBB has a busted feed creator so go through and remove any invalid characters
*
* @param mixed $rawFeed
*/
function FixFeed(&$rawFeed)
{
    $rawFeed = preg_replace('/[^(\x20-\x7F)]*/', '', $rawFeed);
}
?>