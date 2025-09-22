<!--
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-22 16:48:06
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-22 16:48:11
 * @FilePath: /cyclone-HTTPServer/WebApps/MemeServer/front/src/components/HeroSection.vue
 * @Description: 
 * Copyright (c) 2025 by cyclone, All Rights Reserved. 
-->
<template>
  <div class="gradient-bg text-white">
    <div class="max-w-7xl mx-auto py-12 px-4 sm:px-6 lg:px-8">
      <div class="lg:grid lg:grid-cols-2 lg:gap-8 items-center">
        <div class="mb-8 lg:mb-0" data-aos="fade-right">
          <h1 class="text-4xl font-extrabold tracking-tight sm:text-5xl lg:text-6xl mb-4">
            测试你的梗含量
          </h1>
          <p class="text-xl text-purple-100 max-w-3xl">
            你在互联网上是什么等级的冲浪选手？ 来测试一下看看你的排名!
          </p>
          <div class="mt-8">
            <button 
              @click="$emit('start-quiz')"
              class="inline-flex items-center px-6 py-3 border border-transparent text-base font-medium rounded-md text-purple-700 bg-white hover:bg-purple-50">
              开始测验
              <i data-feather="arrow-right" class="ml-2"></i>
            </button>
          </div>
        </div>
        <div class="relative" data-aos="fade-left">
          <img src="http://static.photos/technology/640x360/1" alt="Meme quiz illustration" class="rounded-lg shadow-xl">
          <div class="absolute -bottom-4 -right-4 bg-white rounded-lg shadow-lg p-4">
            <div class="flex items-center">
              <div class="flex-shrink-0">
                <img class="h-10 w-10 rounded-full" src="http://static.photos/people/200x200/2" alt="">
              </div>
              <div class="ml-3">
                <p class="text-sm font-medium text-gray-900">@memelord69</p>
                <p class="text-sm text-purple-600">Top 1% Player</p>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
defineEmits(['start-quiz']);
</script>

<style scoped>
.gradient-bg {
  background: linear-gradient(135deg, #6366f1 0%, #8b5cf6 50%, #d946ef 100%);
}
</style>