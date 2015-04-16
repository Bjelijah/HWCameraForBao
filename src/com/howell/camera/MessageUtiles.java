package com.howell.camera;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class MessageUtiles {
//	private static Context context;
//	public ToastUtiles(Context context){
//		this.context = context;
//	}
	public static void postToast(Context context,String message,int time){
//		Toast toast= Toast.makeText(context, message, 1000);
//		toast.setGravity(Gravity.CENTER, 0, 0);
//		toast.show();
		Toast.makeText(context, message, time).show();
	}
	public static void postAlerDialog(Context context,String message){
		new AlertDialog.Builder(context)   
//        .setTitle("用户名或密码错误")   
        .setMessage(message)                 
        .setPositiveButton("确定", null)   
        .show();  
	}
	
	public static void postNewUIDialog(Context context,String message,String buttonName,final int flag){
		 final Dialog lDialog = new Dialog(context,android.R.style.Theme_Translucent_NoTitleBar_Fullscreen);
//         lDialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
         lDialog.setContentView(R.layout.dialog_view);
//         ((TextView) lDialog.findViewById(R.id.dialog_title)).setText(pTitle);
         ((TextView) lDialog.findViewById(R.id.dialog_message)).setText(message);
         ((Button) lDialog.findViewById(R.id.ok)).setText(buttonName);
         ((Button) lDialog.findViewById(R.id.ok))
                 .setOnClickListener(new OnClickListener() {
                     @Override
                     public void onClick(View v) {
                         // write your code to do things after users clicks OK
                    	 if(flag == 0){
//                    		MyTask mTask = new MyTask();
//             				mTask.execute();
                    	 }
                         lDialog.dismiss();
                     }
                 });
          lDialog.show();
	}
}
