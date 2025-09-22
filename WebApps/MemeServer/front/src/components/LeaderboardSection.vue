<template>
  <div class="py-12 bg-white">
    <div class="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
      <div class="text-center mb-12">
        <h2 class="text-3xl font-extrabold text-gray-900 sm:text-4xl" data-aos="fade-up">
          排行榜
        </h2>
        <p class="mt-4 max-w-2xl text-xl text-gray-500 mx-auto" data-aos="fade-up" data-aos-delay="100">
          看看你在其他网络冲浪选手中的排名如何
        </p>
      </div>
      <div class="mb-8" data-aos="fade-up">
        <div class="sm:hidden">
          <label for="tabs" class="sr-only">Select a tab</label>
          <select 
            id="tabs" 
            v-model="selectedCategory"
            @change="loadLeaderboardData"
            class="block w-full pl-3 pr-10 py-2 text-base border-gray-300 focus:outline-none focus:ring-purple-500 focus:border-purple-500 sm:text-sm rounded-md">
            <option value="">所有分类</option>
            <option value="gaming">游戏</option>
            <option value="social_media">抖音</option>
            <option value="tv_movies">影视 & 综艺</option>
            <option value="streaming">主播</option>
            <option value="global">全球</option>
          </select>
        </div>
        <div class="hidden sm:block">
          <div class="border-b border-gray-200">
            <nav class="-mb-px flex space-x-8">
              <a 
                v-for="category in categories" 
                :key="category.value"
                href="#" 
                @click.prevent="selectCategory(category.value)"
                :class="[
                  selectedCategory === category.value 
                    ? 'border-purple-500 text-purple-600' 
                    : 'border-transparent text-gray-500 hover:text-gray-700 hover:border-gray-300',
                  'whitespace-nowrap py-4 px-1 border-b-2 font-medium text-sm'
                ]">
                {{ category.label }}
              </a>
            </nav>
          </div>
        </div>
      </div>
      <div class="bg-white shadow overflow-hidden sm:rounded-lg" data-aos="fade-up">
        <ul class="divide-y divide-gray-200">
          <li 
            v-for="(ranking, index) in leaderboardData.rankings" 
            :key="ranking.rank"
            :class="{
              'px-6 py-4 bg-purple-50': ranking.rank === 1,
              'px-6 py-4': ranking.rank > 1
            }">
            <div class="flex items-center">
              <div 
                class="flex-shrink-0 h-10 w-10 rounded-full flex items-center justify-center"
                :class="getRankClass(ranking.rank)">
                <span :class="getRankTextColor(ranking.rank)" class="font-bold">{{ ranking.rank }}</span>
              </div>
              <div class="ml-4">
                <div class="flex items-center">
                  <img class="h-8 w-8 rounded-full" :src="`http://static.photos/people/200x200/${(ranking.rank % 6) + 1}`" alt="">
                  <p class="ml-2 text-sm font-medium text-gray-900">{{ ranking.nickname || ranking.user_id }}</p>
                </div>
              </div>
              <div class="ml-auto">
                <div class="flex items-center">
                  <span class="text-sm font-medium text-gray-900">{{ ranking.total_score || 0 }}</span>
                  <span 
                    class="ml-2 inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-medium"
                    :class="getTitleClass(getTitleForScore(ranking.total_score || 0))">
                    <i data-feather="award" class="mr-1 h-3 w-3"></i> {{ getTitleForScore(ranking.total_score || 0) }}
                  </span>
                </div>
              </div>
            </div>
          </li>
        </ul>
        <div class="bg-gray-50 px-4 py-3 flex items-center justify-between border-t border-gray-200 sm:px-6">
          <div class="flex-1 flex justify-between sm:hidden">
            <a href="#" class="relative inline-flex items-center px-4 py-2 border border-gray-300 text-sm font-medium rounded-md text-gray-700 bg-white hover:bg-gray-50">
              上一页
            </a>
            <a href="#" class="ml-3 relative inline-flex items-center px-4 py-2 border border-gray-300 text-sm font-medium rounded-md text-gray-700 bg-white hover:bg-gray-50">
              下一页
            </a>
          </div>
          <div class="hidden sm:flex-1 sm:flex sm:items-center sm:justify-between">
            <div>
              <p class="text-sm text-gray-700">
                显示 <span class="font-medium">1</span> 到 <span class="font-medium">{{ leaderboardData.rankings?.length || 0 }}</span> 共 <span class="font-medium">{{ leaderboardData.total || 0 }}</span> 条结果
              </p>
            </div>
            <div>
              <nav class="relative z-0 inline-flex rounded-md shadow-sm -space-x-px" aria-label="Pagination">
                <a href="#" class="relative inline-flex items-center px-2 py-2 rounded-l-md border border-gray-300 bg-white text-sm font-medium text-gray-500 hover:bg-gray-50">
                  <span class="sr-only">上一页</span>
                  <i data-feather="chevron-left" class="h-5 w-5"></i>
                </a>
                <a href="#" aria-current="page" class="z-10 bg-purple-50 border-purple-500 text-purple-600 relative inline-flex items-center px-4 py-2 border text-sm font-medium">
                  1
                </a>
                <a href="#" class="bg-white border-gray-300 text-gray-500 hover:bg-gray-50 relative inline-flex items-center px-4 py-2 border text-sm font-medium">
                  2
                </a>
                <a href="#" class="bg-white border-gray-300 text-gray-500 hover:bg-gray-50 relative inline-flex items-center px-4 py-2 border text-sm font-medium">
                  3
                </a>
                <span class="relative inline-flex items-center px-4 py-2 border border-gray-300 bg-white text-sm font-medium text-gray-700">
                  ...
                </span>
                <a href="#" class="bg-white border-gray-300 text-gray-500 hover:bg-gray-50 relative inline-flex items-center px-4 py-2 border text-sm font-medium">
                  8
                </a>
                <a href="#" class="bg-white border-gray-300 text-gray-500 hover:bg-gray-50 relative inline-flex items-center px-4 py-2 border text-sm font-medium">
                  9
                </a>
                <a href="#" class="bg-white border-gray-300 text-gray-500 hover:bg-gray-50 relative inline-flex items-center px-4 py-2 border text-sm font-medium">
                  10
                </a>
                <a href="#" class="relative inline-flex items-center px-2 py-2 rounded-r-md border border-gray-300 bg-white text-sm font-medium text-gray-500 hover:bg-gray-50">
                  <span class="sr-only">下一页</span>
                  <i data-feather="chevron-right" class="h-5 w-5"></i>
                </a>
              </nav>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue';

const selectedCategory = ref('');
const leaderboardData = ref({
  rankings: [],
  total: 0,
  page: 1,
  limit: 20
});

const categories = [
  { value: '', label: '所有分类' },
  { value: 'gaming', label: '游戏' },
  { value: 'social_media', label: '抖音' },
  { value: 'tv_movies', label: '影视 & 综艺' },
  { value: 'streaming', label: '主播' },
  { value: 'global', label: '全球' }
];

// API 基础URL
const API_BASE = 'http://129.211.120.83:8080/api';

// API工具函数
const apiRequest = async (endpoint, options = {}) => {
  const url = `${API_BASE}${endpoint}`;
  const config = {
    headers: {
      'Content-Type': 'application/json',
      ...options.headers
    },
    ...options
  };
  try {
    const response = await fetch(url, config);
    const data = await response.json();
    if (!response.ok) {
      throw new Error(data.message || 'API请求失败');
    }
    return data;
  } catch (error) {
    console.error('API请求错误:', error);
    throw error;
  }
};

const loadLeaderboardData = async () => {
  try {
    const params = new URLSearchParams();
    params.append('page', '1');
    params.append('limit', '20');
    
    const endpoint = selectedCategory.value 
      ? `/leaderboard/category/${selectedCategory.value}` 
      : '/leaderboard/total';
    
    const data = await apiRequest(`${endpoint}?${params}`);
    leaderboardData.value = data;
  } catch (error) {
    console.error('加载排行榜失败:', error);
    // 设置默认数据
    leaderboardData.value = {
      rankings: [
        { rank: 1, nickname: 'MemeMaster2000', total_score: 9850 },
        { rank: 2, nickname: 'DankLord', total_score: 8720 },
        { rank: 3, nickname: 'PepeTheFrog', total_score: 7650 },
        { rank: 42, nickname: 'You', total_score: 1850 },
        { rank: 43, nickname: 'WojakWarrior', total_score: 1800 }
      ],
      total: 1234,
      page: 1,
      limit: 20
    };
  }
};

const selectCategory = (category) => {
  selectedCategory.value = category;
  loadLeaderboardData();
};

const getRankClass = (rank) => {
  if (rank === 1) return 'bg-purple-100';
  if (rank === 2) return 'bg-yellow-100';
  if (rank === 3) return 'bg-gray-100';
  return 'bg-gray-100';
};

const getRankTextColor = (rank) => {
  if (rank === 1) return 'text-purple-800';
  if (rank === 2) return 'text-yellow-800';
  if (rank === 3) return 'text-gray-800';
  return 'text-gray-800';
};

const getTitleForScore = (score) => {
  if (score >= 5000) return '传奇';
  if (score >= 3000) return '专家';
  if (score >= 2000) return '高级';
  if (score >= 1000) return '新手';
  return '新手';
};

const getTitleClass = (title) => {
  if (title === '传奇') return 'bg-purple-100 text-purple-800';
  if (title === '专家') return 'bg-yellow-100 text-yellow-800';
  if (title === '高级') return 'bg-gray-100 text-gray-800';
  return 'bg-blue-100 text-blue-800';
};

onMounted(() => {
  loadLeaderboardData();
});
</script>