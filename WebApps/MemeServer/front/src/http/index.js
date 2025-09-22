
import axios from "axios";

import $cookies from "vue-cookies";
// 请求的1 基本配置
const request = axios.create({
  baseURL: "http://127.0.0.1:8080/api/v1",
  timeout: 5000,
  headers: {
    "content-Type": "application/json; charset=utf-8",
  },
});

let loadingInstance;



/*


业务成功
	
response.data.code == 100
	
直接返回
response.data
，调用方可以通过
.then
获取数据。
业务失败
	
response.data.code != 100
	
显示错误提示，返回被拒绝的 Promise，调用方可以通过
.catch
捕获错误信息。
HTTP 错误（401、403 等）
	
error.response.status
存在
	
根据状态码显示对应提示，返回被拒绝的 Promise。
网络错误或其他异常
	
error.response
不存在
	
提示“未知错误”，返回被拒绝的 Promise。
*/

// 导出请求对象
export default request;
