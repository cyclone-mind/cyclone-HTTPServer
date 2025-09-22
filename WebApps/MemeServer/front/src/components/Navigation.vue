<!--
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-22 16:47:54
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-22 16:47:59
 * @FilePath: /cyclone-HTTPServer/WebApps/MemeServer/front/src/components/Navigation.vue
 * @Description: 
 * Copyright (c) 2025 by cyclone, All Rights Reserved. 
-->
<template>
  <nav class="bg-white shadow-sm">
    <div class="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
      <div class="flex justify-between h-16">
        <div class="flex items-center">
          <div class="flex-shrink-0 flex items-center">
            <i data-feather="zap" class="text-purple-600"></i>
            <span class="ml-2 text-xl font-bold text-gray-900">冲浪</span>
          </div>
          <div class="hidden sm:ml-6 sm:flex sm:space-x-8">
            <a href="#" class="border-purple-500 text-gray-900 inline-flex items-center px-1 pt-1 border-b-2 text-sm font-medium">首页</a>
            <a href="#" class="border-transparent text-gray-500 hover:border-gray-300 hover:text-gray-700 inline-flex items-center px-1 pt-1 border-b-2 text-sm font-medium">排行榜</a>
            <a href="#" class="border-transparent text-gray-500 hover:border-gray-300 hover:text-gray-700 inline-flex items-center px-1 pt-1 border-b-2 text-sm font-medium">个人资料</a>
          </div>
        </div>
        <div class="hidden sm:ml-6 sm:flex sm:items-center">
          <button class="bg-white p-1 rounded-full text-gray-400 hover:text-gray-500 focus:outline-none">
            <i data-feather="bell"></i>
          </button>
          <div class="ml-3 relative" v-if="user">
            <div>
              <button class="flex text-sm rounded-full focus:outline-none">
                <img class="h-8 w-8 rounded-full" :src="user.avatar || 'http://static.photos/people/200x200/1'" alt="用户头像">
              </button>
            </div>
          </div>
          <div class="ml-3 relative" v-else>
            <button class="px-4 py-2 text-sm font-medium text-white bg-purple-600 rounded-md hover:bg-purple-700" @click="$emit('login')">
              登录
            </button>
          </div>
        </div>
        <div class="-mr-2 flex items-center sm:hidden">
          <button type="button" class="inline-flex items-center justify-center p-2 rounded-md text-gray-400 hover:text-gray-500 hover:bg-gray-100 focus:outline-none" aria-controls="mobile-menu" aria-expanded="false">
            <i data-feather="menu"></i>
          </button>
        </div>
      </div>
    </div>
  </nav>
</template>

<script setup>
defineProps({
  user: Object
});

defineEmits(['login', 'logout']);
</script>