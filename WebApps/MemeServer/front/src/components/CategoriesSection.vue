<!--
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-22 16:48:19
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-22 16:48:24
 * @FilePath: /cyclone-HTTPServer/WebApps/MemeServer/front/src/components/CategoriesSection.vue
 * @Description: 
 * Copyright (c) 2025 by cyclone, All Rights Reserved. 
-->
<template>
  <div class="py-12 bg-white">
    <div class="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
      <div class="text-center mb-12">
        <h2 class="text-3xl font-extrabold text-gray-900 sm:text-4xl" data-aos="fade-up">
          选择梗分类
        </h2>
        <p class="mt-4 max-w-2xl text-xl text-gray-500 mx-auto" data-aos="fade-up" data-aos-delay="100">
          测试你的不同风格的梗的水平 或无所畏惧，来就有(接受混合挑战)
        </p>
      </div>
      <div class="grid grid-cols-1 gap-8 sm:grid-cols-2 lg:grid-cols-3">
        <div 
          v-for="(category, index) in categories" 
          :key="category.name"
          class="category-card bg-white overflow-hidden shadow rounded-lg"
          :data-aos="`fade-up`"
          :data-aos-delay="`${(index + 2) * 100}`"
          @click="selectCategory(category.key)"
        >
          <div class="px-4 py-5 sm:p-6">
            <div class="flex items-center">
              <div class="flex-shrink-0" :class="`bg-${category.color}-500 rounded-md p-3`">
                <i :data-feather="category.icon" class="text-white"></i>
              </div>
              <div class="ml-5 w-0 flex-1">
                <h3 class="text-lg font-medium text-gray-900">{{ category.name }}</h3>
                <p class="mt-1 text-sm text-gray-500">{{ category.description }}</p>
              </div>
            </div>
            <div class="mt-6">
              <button 
                :class="`inline-flex items-center px-4 py-2 border border-transparent text-sm font-medium rounded-md shadow-sm text-white bg-${category.color}-600 hover:bg-${category.color}-700`">
                开始测验 ( 10 条)
              </button>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { defineEmits } from 'vue';

const emit = defineEmits(['select-category']);

const categories = [
  { name: '游戏', key: 'gaming', color: 'purple', icon: 'cpu', description: '从瓦到LPL' },
  { name: '社交媒体', key: 'social_media', color: 'pink', icon: 'video', description: '流行趋势 & 挑战' },
  { name: '影视 & 综艺', key: 'tv_movies', color: 'indigo', icon: 'tv', description: '标志性场景 & 引用语' },
  { name: '直播', key: 'streaming', color: 'yellow', icon: 'mic', description: '抖音 & 虎牙' },
  { name: '全球', key: 'global', color: 'green', icon: 'globe', description: '世界范围广泛使用' },
  { name: '随机', key: '', color: 'red', icon: 'shuffle', description: '所有条目总和' }
];

const selectCategory = (categoryKey) => {
  emit('select-category', categoryKey);
};
</script>

<style scoped>
.category-card {
  transition: all 0.3s ease;
}
.category-card:hover {
  transform: translateY(-5px);
  box-shadow: 0 20px 25px -5px rgba(0, 0, 0, 0.1), 0 10px 10px -5px rgba(0, 0, 0, 0.04);
}
</style>