<?php
//接收微信后台的消息并回复
//没有校验signature，懒得。。
	file_put_contents("log",date('Y-m-d H:i:s',time())." A REQUEST\n",FILE_APPEND);
    $postArr = $HTTP_RAW_POST_DATA;
	$postObj = simplexml_load_string( $postArr );
	$toUser   = $postObj->FromUserName;
	$fromUser = $postObj->ToUserName;
	$time     = time();
	$msgType  =  'text';
	$template = 
	"<xml>
	<ToUserName><![CDATA[%s]]></ToUserName>
	<FromUserName><![CDATA[%s]]></FromUserName>
	<CreateTime>%s</CreateTime>
	<MsgType><![CDATA[%s]]></MsgType>
	<Content><![CDATA[%s]]></Content>
	<FuncFlag>0</FuncFlag>
	</xml>";
	if( strtolower( $postObj->MsgType) == 'event'){
		if( strtolower($postObj->Event == 'subscribe') ){
			$content  = '欢迎关注！快来发送"get"，读取精确度数吧！[Joyful]';
			$info = sprintf($template, $toUser, $fromUser, $time, $msgType, $content);
			echo $info;
			return;
		}
	}
	if(strtolower($postObj->MsgType)=='text'){
		// file_put_contents("log","Received a text\n",FILE_APPEND);
		// file_put_contents("log","text=".$postObj->Content."\n",FILE_APPEND);
		if(strtolower($postObj->Content)=='get'){
			// file_put_contents("log","Received a get\n",FILE_APPEND);
			$content=file_get_contents("data");
			$info = sprintf($template, $toUser, $fromUser, $time, $msgType, $content);
			echo $info;
			return;
		}
	}
	$content='[Shocked]';
	$info = sprintf($template, $toUser, $fromUser, $time, $msgType, $content);
	echo $info;
	return;	
	// file_put_contents("log",$postObj->FromUserName."\n",FILE_APPEND);
?>
