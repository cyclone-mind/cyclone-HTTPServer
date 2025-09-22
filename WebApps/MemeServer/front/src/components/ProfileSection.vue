<template>
  <div class="py-12 bg-gray-50">
    <div class="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
      <div class="lg:grid lg:grid-cols-3 lg:gap-8">
        <div class="lg:col-span-1" data-aos="fade-right">
          <div class="bg-white shadow rounded-lg overflow-hidden">
            <div class="px-4 py-5 sm:px-6">
              <div class="flex items-center">
                <div class="flex-shrink-0">
                  <img class="h-16 w-16 rounded-full" :src="user?.avatar || 'http://static.photos/people/200x200/1'" alt="">
                </div>
                <div class="ml-4">
                  <h3 class="text-lg font-medium text-gray-900">{{ user?.nickname || '梗爱好者' }}</h3>
                  <p class="text-sm text-gray-500">{{ joinDate }}</p>
                </div>
              </div>
            </div>
            <div class="border-t border-gray-200 px-4 py-5 sm:px-6">
              <dl class="grid grid-cols-1 gap-x-4 gap-y-8 sm:grid-cols-2">
                <div class="sm:col-span-1">
                  <dt class="text-sm font-medium text-gray-500">总分</dt>
                  <dd class="mt-1 text-sm text-gray-900">{{ user?.total_score || 1850 }}</dd>
                </div>
                <div class="sm:col-span-1">
                  <dt class="text-sm font-medium text-gray-500">测试次数</dt>
                  <dd class="mt-1 text-sm text-gray-900">{{ user?.test_count || 12 }}</dd>
                </div>
                <div class="sm:col-span-1">
                  <dt class="text-sm font-medium text-gray-500">正确答案</dt>
                  <dd class="mt-1 text-sm text-gray-900">{{ user?.correct_answers || 87 }}</dd>
                </div>
                <div class="sm:col-span-1">
                  <dt class="text-sm font-medium text-gray-500">准确率</dt>
                  <dd class="mt-1 text-sm text-gray-900">{{ user?.accuracy || '72.5%' }}</dd>
                </div>
              </dl>
            </div>
            <div class="px-4 py-4 sm:px-6 bg-gray-50">
              <div class="text-center">
                <button class="inline-flex items-center px-4 py-2 border border-transparent text-sm font-medium rounded-md shadow-sm text-white bg-purple-600 hover:bg-purple-700">
                  <i data-feather="edit" class="mr-2"></i> 编辑资料
                </button>
              </div>
            </div>
          </div>
          <div class="bg-white shadow rounded-lg overflow-hidden mt-8">
            <div class="px-4 py-5 sm:px-6 border-b border-gray-200">
              <h3 class="text-lg font-medium text-gray-900">你的称号</h3>
            </div>
            <div class="px-4 py-5 sm:p-6">
              <div class="text-center mb-4">
                <div class="inline-flex items-center px-3 py-1 rounded-full text-sm font-medium bg-gradient-to-r from-purple-500 to-pink-500 text-white">
                  <span>{{ userTitle }}</span>
                </div>
              </div>
              <p class="text-center text-sm text-gray-500">通过参加测验并提高分数来获得更多称号！</p>
              <div class="mt-6 grid grid-cols-2 gap-4">
                <div class="text-center">
                  <div class="inline-flex items-center justify-center h-10 w-10 rounded-full bg-gray-100">
                    <i data-feather="lock" class="text-gray-400"></i>
                  </div>
                  <p class="mt-1 text-xs text-gray-500">梗领主 (5000+)</p>
                </div>
                <div class="text-center">
                  <div class="inline-flex items-center justify-center h-10 w-10 rounded-full bg-gray-100">
                    <i data-feather="lock" class="text-gray-400"></i>
                  </div>
                  <p class="mt-1 text-xs text-gray-500">黑暗大师 (3000+)</p>
                </div>
                <div class="text-center">
                  <div class="inline-flex items-center justify-center h-10 w-10 rounded-full bg-gray-100">
                    <i data-feather="lock" class="text-gray-400"></i>
                  </div>
                  <p class="mt-1 text-xs text-gray-500">病毒专家 (2000+)</p>
                </div>
                <div class="text-center">
                  <div class="inline-flex items-center justify-center h-10 w-10 rounded-full bg-gray-100">
                    <i data-feather="lock" class="text-gray-400"></i>
                  </div>
                  <p class="mt-1 text-xs text-gray-500">潮流引领者 (1000+)</p>
                </div>
              </div>
            </div>
          </div>
        </div>
        <div class="mt-8 lg:mt-0 lg:col-span-2" data-aos="fade-left">
          <div class="bg-white shadow rounded-lg overflow-hidden">
            <div class="px-4 py-5 sm:px-6 border-b border-gray-200">
              <h3 class="text-lg font-medium text-gray-900">你的测试历史</h3>
            </div>
            <div class="px-4 py-5 sm:p-6">
              <div class="overflow-x-auto">
                <table class="min-w-full divide-y divide-gray-200">
                  <thead class="bg-gray-50">
                    <tr>
                      <th scope="col" class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">日期</th>
                      <th scope="col" class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">分类</th>
                      <th scope="col" class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">分数</th>
                      <th scope="col" class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">正确</th>
                      <th scope="col" class="relative px-6 py-3">
                        <span class="sr-only">查看</span>
                      </th>
                    </tr>
                  </thead>
                  <tbody class="bg-white divide-y divide-gray-200">
                    <tr v-for="(test, index) in testHistory" :key="index">
                      <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-500">{{ test.date }}</td>
                      <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900">{{ test.category }}</td>
                      <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900">{{ test.score }}</td>
                      <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900">{{ test.correct }}</td>
                      <td class="px-6 py-4 whitespace-nowrap text-right text-sm font-medium">
                        <a href="#" class="text-purple-600 hover:text-purple-900">详情</a>
                      </td>
                    </tr>
                  </tbody>
                </table>
              </div>
              <div class="mt-4 flex justify-between items-center">
                <div>
                  <p class="text-sm text-gray-700">
                    显示 <span class="font-medium">1</span> 到 <span class="font-medium">{{ testHistory.length }}</span> 共 <span class="font-medium">12</span> 条结果
                  </p>
                </div>
                <div class="flex space-x-2">
                  <button class="inline-flex items-center px-3 py-1 border border-gray-300 text-sm font-medium rounded-md text-gray-700 bg-white hover:bg-gray-50">
                    <i data-feather="chevron-left" class="h-4 w-4"></i>
                  </button>
                  <button class="inline-flex items-center px-3 py-1 border border-gray-300 text-sm font-medium rounded-md text-gray-700 bg-white hover:bg-gray-50">
                    <i data-feather="chevron-right" class="h-4 w-4"></i>
                  </button>
                </div>
              </div>
            </div>
          </div>
          <div class="bg-white shadow rounded-lg overflow-hidden mt-8">
            <div class="px-4 py-5 sm:px-6 border-b border-gray-200">
              <h3 class="text-lg font-medium text-gray-900">表现概览</h3>
            </div>
            <div class="px-4 py-5 sm:p-6">
              <div class="h-64">
                <!-- Chart placeholder - would be replaced with actual chart in implementation -->
                <div class="flex items-center justify-center h-full bg-gray-50 rounded-lg border border-gray-200">
                  <p class="text-gray-500">表现图表将显示在这里</p>
                </div>
              </div>
              <div class="mt-6 grid grid-cols-1 gap-5 sm:grid-cols-3">
                <div class="bg-white overflow-hidden shadow rounded-lg">
                  <div class="px-4 py-5 sm:p-6">
                    <div class="flex items-center">
                      <div class="flex-shrink-0 bg-purple-500 rounded-md p-3">
                        <i data-feather="trending-up" class="text-white"></i>
                      </div>
                      <div class="ml-5 w-0 flex-1">
                        <dt class="text-sm font-medium text-gray-500 truncate">最佳分类</dt>
                        <dd class="flex items-baseline">
                          <div class="text-2xl font-semibold text-gray-900">游戏</div>
                        </dd>
                      </div>
                    </div>
                  </div>
                </div>
                <div class="bg-white overflow-hidden shadow rounded-lg">
                  <div class="px-4 py-5 sm:p-6">
                    <div class="flex items-center">
                      <div class="flex-shrink-0 bg-yellow-500 rounded-md p-3">
                        <i data-feather="trending-down" class="text-white"></i>
                      </div>
                      <div class="ml-5 w-0 flex-1">
                        <dt class="text-sm font-medium text-gray-500 truncate">最差分类</dt>
                        <dd class="flex items-baseline">
                          <div class="text-2xl font-semibold text-gray-900">影视 & 综艺</div>
                        </dd>
                      </div>
                    </div>
                  </div>
                </div>
                <div class="bg-white overflow-hidden shadow rounded-lg">
                  <div class="px-4 py-5 sm:p-6">
                    <div class="flex items-center">
                      <div class="flex-shrink-0 bg-green-500 rounded-md p-3">
                        <i data-feather="bar-chart-2" class="text-white"></i>
                      </div>
                      <div class="ml-5 w-0 flex-1">
                        <dt class="text-sm font-medium text-gray-500 truncate">平均分数</dt>
                        <dd class="flex items-baseline">
                          <div class="text-2xl font-semibold text-gray-900">72.5%</div>
                        </dd>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { computed } from 'vue';

const props = defineProps({
  user: Object
});

const joinDate = computed(() => {
  return props.user?.join_date || '3个月前加入';
});

const userTitle = computed(() => {
  const score = props.user?.total_score || 0;
  if (score >= 5000) return '传奇';
  if (score >= 3000) return '专家';
  if (score >= 2000) return '高级';
  if (score >= 1000) return '新手';
  return '新手';
});

const testHistory = [
  { date: 'Today, 15:42', category: '游戏', score: '85/100', correct: '8/10' },
  { date: 'Yesterday, 19:15', category: '抖音', score: '65/100', correct: '6/10' },
  { date: '2 days ago', category: '主播', score: '75/100', correct: '7/10' },
  { date: '1 week ago', category: '随机混合', score: '120/200', correct: '12/20' },
  { date: '2 weeks ago', category: '影视 & 综艺', score: '55/100', correct: '5/10' }
];
</script>