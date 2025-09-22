/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-22 15:46:11
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-22 16:38:34
 * @FilePath: /cyclone-HTTPServer/WebApps/MemeServer/front/src/router/index.js
 * @Description: 
 * Copyright (c) 2025 by cyclone, All Rights Reserved. 
 */
import { createRouter, createWebHistory } from 'vue-router'
import HomeView from '../views/HomeView.vue'
import LoginView from '../views/LoginView.vue'

const routes = [
  {
    path: '/',
    name: 'Home',
    component: HomeView
  },
  {
    path: '/login',
    name: 'Login',
    component: LoginView
  }
]

const router = createRouter({
  history: createWebHistory(),
  routes
})

export default router
