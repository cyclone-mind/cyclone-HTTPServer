import { createApp } from 'vue';
import App from './App.vue';
import './assets/styles/global.css';

// 引入 Tailwind CSS
import 'https://cdn.tailwindcss.com';

// 引入 AOS 动画库
import AOS from 'https://unpkg.com/aos@2.3.1/dist/aos.js';
import 'https://unpkg.com/aos@2.3.1/dist/aos.css';

// 引入 Feather Icons
import 'https://cdn.jsdelivr.net/npm/feather-icons/dist/feather.min.js';
import 'https://unpkg.com/feather-icons';

const app = createApp(App);

app.mount('#app');

// 初始化 AOS
AOS.init();

// 初始化 Feather Icons
if (window.feather) {
  window.feather.replace();
}