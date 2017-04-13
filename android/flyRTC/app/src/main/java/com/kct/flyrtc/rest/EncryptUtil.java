package com.kct.flyrtc.rest;

import java.security.MessageDigest;

/**
 * Created by zhouwq on 2017/3/8/008.
 * 加解密类
 */

public class EncryptUtil {
    private static final String UTF8 = "utf-8";

    // MD5数字签名
    public String md5Digest(String src) throws Exception {
        // 定义数字签名方法, 可用：MD5, SHA-1
        MessageDigest md = MessageDigest.getInstance("MD5");
        byte[] b = md.digest(src.getBytes(UTF8));
        return Byte2HexStr(b);
    }

    // BASE64编码
    public String base64Encoder(String src) throws Exception {
        return Base64.encode(src.getBytes(UTF8));
    }

    // BASE64解码
    public String base64Decoder(String dest) throws Exception {
        return new String(Base64.decode(dest));
    }

    // 字节数组转化为大写16进制字符串
    private String Byte2HexStr(byte[] b) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < b.length; i++) {
            String s = Integer.toHexString(b[i] & 0xFF);
            if (s.length() == 1) {
                sb.append("0");
            }
            sb.append(s.toUpperCase());
        }
        return sb.toString();
    }
}
