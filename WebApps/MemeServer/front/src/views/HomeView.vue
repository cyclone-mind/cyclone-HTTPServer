<template>
  <div class="min-h-screen">
    <!-- Navigation -->
    <nav class="bg-white shadow-sm">
      <div class="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
        <div class="flex justify-between h-16">
          <div class="flex items-center">
            <div class="flex-shrink-0 flex items-center">
              <i data-feather="zap" class="text-purple-600"></i>
              <span class="ml-2 text-xl font-bold text-gray-900">冲浪</span>
            </div>
            <div class="hidden sm:ml-6 sm:flex sm:space-x-8">
              <router-link to="/" class="border-purple-500 text-gray-900 inline-flex items-center px-1 pt-1 border-b-2 text-sm font-medium">首页</router-link>
              <a href="#leaderboard" class="border-transparent text-gray-500 hover:border-gray-300 hover:text-gray-700 inline-flex items-center px-1 pt-1 border-b-2 text-sm font-medium">排行榜</a>
              <a href="#profile" class="border-transparent text-gray-500 hover:border-gray-300 hover:text-gray-700 inline-flex items-center px-1 pt-1 border-b-2 text-sm font-medium">个人资料</a>
            </div>
          </div>
          <div class="hidden sm:ml-6 sm:flex sm:items-center">
            <button class="bg-white p-1 rounded-full text-gray-400 hover:text-gray-500 focus:outline-none">
              <i data-feather="bell"></i>
            </button>
            <div class="ml-3 relative">
              <div>
                <button v-if="currentUser" class="flex text-sm rounded-full focus:outline-none" @click="logout">
                  <img class="h-8 w-8 rounded-full" :src="currentUser.avatar || 'http://static.photos/people/200x200/1'" alt="">
                </button>
                <router-link v-else to="/login" class="inline-flex items-center px-4 py-2 border border-transparent text-sm font-medium rounded-md text-purple-700 bg-white hover:bg-purple-50">
                  登录
                </router-link>
              </div>
            </div>
          </div>
        </div>
      </div>
    </nav>

    <!-- Hero Section -->
    <div class="gradient-bg text-white">
      <div class="max-w-7xl mx-auto py-12 px-4 sm:px-6 lg:px-8">
        <div class="lg:grid lg:grid-cols-2 lg:gap-8 items-center">
          <div class="mb-8 lg:mb-0">
            <h1 class="text-4xl font-extrabold tracking-tight sm:text-5xl lg:text-6xl mb-4">
              测试你的梗含量
            </h1>
            <p class="text-xl text-purple-100 max-w-3xl">
              你在互联网上是什么等级的冲浪选手？ 来测试一下看看你的排名!
            </p>
            <div class="mt-8">
              <button @click="scrollToCategories" class="inline-flex items-center px-6 py-3 border border-transparent text-base font-medium rounded-md text-purple-700 bg-white hover:bg-purple-50">
                开始测验
                <i data-feather="arrow-right" class="ml-2"></i>
              </button>
            </div>
          </div>
          <div class="relative">
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

    <!-- Categories Section -->
    <div id="categories" class="py-12 bg-white">
      <div class="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
        <div class="text-center mb-12">
          <h2 class="text-3xl font-extrabold text-gray-900 sm:text-4xl">
            选择梗分类
          </h2>
          <p class="mt-4 max-w-2xl text-xl text-gray-500 mx-auto">
            测试你的不同风格的梗的水平 或无所畏惧，来就有(接受混合挑战)
          </p>
        </div>
        <div class="grid grid-cols-1 gap-8 sm:grid-cols-2 lg:grid-cols-3">
          <div v-for="category in categories" :key="category.name" class="category-card bg-white overflow-hidden shadow rounded-lg">
            <div class="px-4 py-5 sm:p-6">
              <div class="flex items-center">
                <div class="flex-shrink-0 rounded-md p-3" :class="category.bgColor">
                  <i :data-feather="category.icon" class="text-white"></i>
                </div>
                <div class="ml-5 w-0 flex-1">
                  <h3 class="text-lg font-medium text-gray-900">{{ category.name }}</h3>
                  <p class="mt-1 text-sm text-gray-500">{{ category.description }}</p>
                </div>
              </div>
              <div class="mt-6">
                <button @click="startQuiz(category.key)" class="inline-flex items-center px-4 py-2 border border-transparent text-sm font-medium rounded-md shadow-sm text-white hover:opacity-90" :class="category.buttonColor">
                  开始测验 ( 10 条)
                </button>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>

    <!-- Quiz Section -->
    <div v-if="showQuiz" class="py-12 bg-gray-50">
      <div class="max-w-3xl mx-auto px-4 sm:px-6 lg:px-8">
        <div class="bg-white shadow rounded-lg overflow-hidden">
          <div class="p-6">
            <!-- Progress Bar -->
            <div class="mb-6">
              <div class="flex justify-between items-center mb-2">
                <span class="text-sm font-medium text-gray-700">问题 {{ currentQuestionIndex + 1 }}/{{ totalQuestions }}</span>
                <span class="text-sm font-medium text-purple-600">分数: {{ currentScore }}</span>
              </div>
              <div class="w-full bg-gray-200 rounded-full h-2.5">
                <div class="bg-purple-600 h-2.5 rounded-full" :style="{ width: progressPercentage + '%' }"></div>
              </div>
            </div>
            
            <!-- Question -->
            <div class="mb-8" v-if="currentQuestion">
              <h3 class="text-xl font-medium text-gray-900 mb-4">{{ currentQuestion.question_text }}</h3>
              <div class="grid grid-cols-1 gap-4">
                <div v-for="(option, key) in currentQuestion.options" :key="key" 
                     @click="selectAnswer(key)"
                     class="option-card border border-gray-200 rounded-lg p-4 cursor-pointer"
                     :class="{ 'selected': selectedAnswer === key }">
                  <div class="flex items-center">
                    <div class="flex-shrink-0 h-5 w-5 rounded-full border border-gray-300 flex items-center justify-center mr-3">
                      <span class="text-xs font-medium">{{ key }}</span>
                    </div>
                    <p class="text-gray-700">{{ option }}</p>
                  </div>
                </div>
              </div>
            </div>
            
            <!-- Navigation -->
            <div class="flex justify-between">
              <button @click="previousQuestion" :disabled="currentQuestionIndex === 0" class="inline-flex items-center px-4 py-2 border border-gray-300 shadow-sm text-sm font-medium rounded-md text-gray-700 bg-white hover:bg-gray-50 disabled:opacity-50">
                <i data-feather="chevron-left" class="mr-2"></i> 上一页
              </button>
              <button @click="nextQuestion" class="inline-flex items-center px-4 py-2 border border-transparent text-sm font-medium rounded-md shadow-sm text-white bg-purple-600 hover:bg-purple-700">
                {{ currentQuestionIndex === totalQuestions - 1 ? '完成测验' : '下一页' }} 
                <i data-feather="chevron-right" class="ml-2"></i>
              </button>
            </div>
          </div>
        </div>
      </div>
    </div>

    <!-- Results Section -->
    <div v-if="showResults" class="py-12 bg-gray-50">
      <div class="max-w-3xl mx-auto px-4 sm:px-6 lg:px-8">
        <div class="bg-white shadow rounded-lg overflow-hidden">
          <div class="p-6">
            <div class="text-center mb-8">
              <div class="mx-auto flex items-center justify-center h-24 w-24 rounded-full bg-purple-100 mb-4">
                <svg class="h-12 w-12 text-purple-600" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                  <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M5 3v4M3 5h4M6 17v4m-2-2h4m5-16l2.286 6.857L21 12l-5.714 2.143L13 21l-2.286-6.857L5 12l5.714-2.143L13 3z" />
                </svg>
              </div>
              <h2 class="text-2xl font-bold text-gray-900 mb-2">测验完成！</h2>
              <p class="text-gray-600">你获得了 <span class="font-bold text-purple-600">{{ finalScore }}/{{ totalQuestions * 10 }}</span> 分</p>
            </div>
            
            <div class="mb-8">
              <div class="flex items-center justify-center mb-6">
                <div class="relative">
                  <svg class="w-32 h-32" viewBox="0 0 36 36">
                    <path d="M18 2.0845 a 15.9155 15.9155 0 0 1 0 31.831 a 15.9155 15.9155 0 0 1 0 -31.831"
                          fill="none" stroke="#e5e7eb" stroke-width="3" stroke-dasharray="100, 100" />
                    <path d="M18 2.0845 a 15.9155 15.9155 0 0 1 0 31.831 a 15.9155 15.9155 0 0 1 0 -31.831"
                          fill="none" stroke="#8b5cf6" stroke-width="3" 
                          :stroke-dasharray="`${scorePercentage}, 100`"
                          class="progress-ring__circle" />
                  </svg>
                  <div class="absolute inset-0 flex items-center justify-center">
                    <span class="text-3xl font-bold text-gray-900">{{ Math.round(scorePercentage) }}%</span>
                  </div>
                </div>
              </div>
              <div class="grid grid-cols-3 gap-4 text-center">
                <div>
                  <p class="text-sm text-gray-500">正确</p>
                  <p class="text-lg font-bold text-green-600">{{ correctAnswers }}</p>
                </div>
                <div>
                  <p class="text-sm text-gray-500">错误</p>
                  <p class="text-lg font-bold text-red-600">{{ wrongAnswers }}</p>
                </div>
                <div>
                  <p class="text-sm text-gray-500">跳过</p>
                  <p class="text-lg font-bold text-yellow-600">0</p>
                </div>
              </div>
            </div>
            
            <div class="grid grid-cols-2 gap-4">
              <button @click="restartQuiz" class="inline-flex items-center justify-center px-4 py-2 border border-gray-300 shadow-sm text-sm font-medium rounded-md text-gray-700 bg-white hover:bg-gray-50">
                <i data-feather="refresh-cw" class="mr-2"></i> 重新测验
              </button>
              <button @click="scrollToLeaderboard" class="inline-flex items-center justify-center px-4 py-2 border border-transparent text-sm font-medium rounded-md shadow-sm text-white bg-purple-600 hover:bg-purple-700">
                <i data-feather="award" class="mr-2"></i> 查看排行榜
              </button>
            </div>
          </div>
        </div>
      </div>
    </div>

    <!-- Leaderboard Section -->
    <div id="leaderboard" class="py-12 bg-white">
      <div class="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
        <div class="text-center mb-12">
          <h2 class="text-3xl font-extrabold text-gray-900 sm:text-4xl">排行榜</h2>
          <p class="mt-4 max-w-2xl text-xl text-gray-500 mx-auto">看看你在其他网络冲浪选手中的排名如何</p>
        </div>
        
        <div class="bg-white shadow overflow-hidden sm:rounded-lg">
          <ul class="divide-y divide-gray-200">
            <li v-for="(player, index) in leaderboard" :key="index" class="px-6 py-4" :class="{ 'bg-purple-50': index === 0, 'bg-blue-50 border-t-2 border-b-2 border-blue-200': player.isCurrentUser }">
              <div class="flex items-center">
                <div class="flex-shrink-0 h-10 w-10 rounded-full flex items-center justify-center" :class="getRankBadgeClass(index + 1)">
                  <span class="font-bold" :class="getRankTextClass(index + 1)">{{ index + 1 }}</span>
                </div>
                <div class="ml-4">
                  <div class="flex items-center">
                    <img class="h-8 w-8 rounded-full" :src="player.avatar" alt="">
                    <p class="ml-2 text-sm font-medium text-gray-900">{{ player.name }}</p>
                  </div>
                </div>
                <div class="ml-auto">
                  <div class="flex items-center">
                    <span class="text-sm font-medium text-gray-900">{{ player.score }}</span>
                    <span class="ml-2 inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-medium" :class="getTitleBadgeClass(player.score)">
                      <i data-feather="award" class="mr-1 h-3 w-3"></i> {{ getTitleForScore(player.score) }}
                    </span>
                  </div>
                </div>
              </div>
            </li>
          </ul>
        </div>
      </div>
    </div>

    <!-- Footer -->
    <footer class="bg-white">
      <div class="max-w-7xl mx-auto py-12 px-4 overflow-hidden sm:px-6 lg:px-8">
        <nav class="-mx-5 -my-2 flex flex-wrap justify-center">
          <div class="px-5 py-2">
            <a href="#" class="text-base text-gray-500 hover:text-gray-900">关于</a>
          </div>
          <div class="px-5 py-2">
            <a href="#" class="text-base text-gray-500 hover:text-gray-900">博客</a>
          </div>
          <div class="px-5 py-2">
            <a href="#" class="text-base text-gray-500 hover:text-gray-900">隐私</a>
          </div>
          <div class="px-5 py-2">
            <a href="#" class="text-base text-gray-500 hover:text-gray-900">条款</a>
          </div>
        </nav>
        <p class="mt-8 text-center text-base text-gray-400">
          &copy; 2023 梗测验。保留所有权利。
        </p>
      </div>
    </footer>
  </div>
</template>

<script setup>
import { ref, computed, onMounted, nextTick } from 'vue'
import { useRouter } from 'vue-router'

const router = useRouter()

// 响应式数据
const currentUser = ref(null)
const showQuiz = ref(false)
const showResults = ref(false)
const currentQuestionIndex = ref(0)
const selectedAnswer = ref('')
const currentScore = ref(0)
const userAnswers = ref([])
const currentQuestions = ref([])

// 分类数据
const categories = ref([
  {
    name: '游戏',
    key: 'gaming',
    description: '从瓦到LPL',
    icon: 'cpu',
    bgColor: 'bg-purple-500',
    buttonColor: 'bg-purple-600 hover:bg-purple-700'
  },
  {
    name: '社交媒体',
    key: 'social_media',
    description: '流行趋势 & 挑战',
    icon: 'video',
    bgColor: 'bg-pink-500',
    buttonColor: 'bg-pink-600 hover:bg-pink-700'
  },
  {
    name: '影视 & 综艺',
    key: 'tv_movies',
    description: '标志性场景 & 引用语',
    icon: 'tv',
    bgColor: 'bg-indigo-500',
    buttonColor: 'bg-indigo-600 hover:bg-indigo-700'
  },
  {
    name: '直播',
    key: 'streaming',
    description: '抖音 & 虎牙',
    icon: 'mic',
    bgColor: 'bg-yellow-500',
    buttonColor: 'bg-yellow-600 hover:bg-yellow-700'
  },
  {
    name: '全球',
    key: 'global',
    description: '世界范围广泛使用',
    icon: 'globe',
    bgColor: 'bg-green-500',
    buttonColor: 'bg-green-600 hover:bg-green-700'
  },
  {
    name: '随机',
    key: 'random',
    description: '所有条目总和',
    icon: 'shuffle',
    bgColor: 'bg-red-500',
    buttonColor: 'bg-red-600 hover:bg-red-700'
  }
])

// 排行榜数据
const leaderboard = ref([
  { name: 'MemeMaster2000', score: 9850, avatar: 'http://static.photos/people/200x200/3' },
  { name: 'DankLord', score: 8720, avatar: 'http://static.photos/people/200x200/4' },
  { name: 'PepeTheFrog', score: 7650, avatar: 'http://static.photos/people/200x200/5' },
  { name: 'You', score: 1850, avatar: 'http://static.photos/people/200x200/1', isCurrentUser: true },
  { name: 'WojakWarrior', score: 1800, avatar: 'http://static.photos/people/200x200/6' }
])

// 示例题目数据
const sampleQuestions = ref([
  {
    id: 1,
    question_text: '哪个流行的Twitch主播以"PogChamp"表情包而闻名？',
    options: {
      A: 'Ninja',
      B: 'Pokimane',
      C: 'xQc',
      D: 'Gootecks'
    },
    correct_answer: 'D'
  },
  {
    id: 2,
    question_text: '以下哪个梗来自于游戏《英雄联盟》？',
    options: {
      A: '666',
      B: 'GG',
      C: '瓦',
      D: 'YYDS'
    },
    correct_answer: 'C'
  }
])

// 计算属性
const totalQuestions = computed(() => currentQuestions.value.length)
const currentQuestion = computed(() => currentQuestions.value[currentQuestionIndex.value])
const progressPercentage = computed(() => ((currentQuestionIndex.value + 1) / totalQuestions.value) * 100)
const finalScore = computed(() => {
  const correct = userAnswers.value.filter(answer => answer.isCorrect).length
  return correct * 10
})
const correctAnswers = computed(() => userAnswers.value.filter(answer => answer.isCorrect).length)
const wrongAnswers = computed(() => userAnswers.value.filter(answer => !answer.isCorrect).length)
const scorePercentage = computed(() => (finalScore.value / (totalQuestions.value * 10)) * 100)

// 方法
const scrollToCategories = () => {
  document.getElementById('categories').scrollIntoView({ behavior: 'smooth' })
}

const scrollToLeaderboard = () => {
  document.getElementById('leaderboard').scrollIntoView({ behavior: 'smooth' })
}

const startQuiz = (categoryKey) => {
  // 重置测验状态
  currentQuestionIndex.value = 0
  selectedAnswer.value = ''
  currentScore.value = 0
  userAnswers.value = []
  currentQuestions.value = [...sampleQuestions.value]
  
  showQuiz.value = true
  showResults.value = false
  
  nextTick(() => {
    document.querySelector('.py-12.bg-gray-50').scrollIntoView({ behavior: 'smooth' })
    if (window.feather) {
      window.feather.replace()
    }
  })
}

const selectAnswer = (answer) => {
  selectedAnswer.value = answer
}

const nextQuestion = () => {
  if (!selectedAnswer.value) {
    alert('请先选择答案')
    return
  }

  // 记录答案
  const isCorrect = selectedAnswer.value === currentQuestion.value.correct_answer
  userAnswers.value.push({
    questionId: currentQuestion.value.id,
    selectedAnswer: selectedAnswer.value,
    correctAnswer: currentQuestion.value.correct_answer,
    isCorrect
  })

  if (isCorrect) {
    currentScore.value += 10
  }

  // 检查是否是最后一题
  if (currentQuestionIndex.value === totalQuestions.value - 1) {
    // 完成测验
    showQuiz.value = false
    showResults.value = true
    nextTick(() => {
      document.querySelector('.py-12.bg-gray-50').scrollIntoView({ behavior: 'smooth' })
      if (window.feather) {
        window.feather.replace()
      }
    })
  } else {
    // 下一题
    currentQuestionIndex.value++
    selectedAnswer.value = ''
  }
}

const previousQuestion = () => {
  if (currentQuestionIndex.value > 0) {
    currentQuestionIndex.value--
    selectedAnswer.value = ''
  }
}

const restartQuiz = () => {
  showResults.value = false
  scrollToCategories()
}

const getRankBadgeClass = (rank) => {
  if (rank === 1) return 'bg-purple-100'
  if (rank === 2) return 'bg-yellow-100'
  if (rank === 3) return 'bg-gray-100'
  return 'bg-gray-50'
}

const getRankTextClass = (rank) => {
  if (rank === 1) return 'text-purple-800'
  if (rank === 2) return 'text-yellow-800'
  if (rank === 3) return 'text-gray-800'
  return 'text-gray-600'
}

const getTitleForScore = (score) => {
  if (score >= 5000) return '传奇'
  if (score >= 3000) return '专家'
  if (score >= 2000) return '高级'
  if (score >= 1000) return '新手'
  return '新手'
}

const getTitleBadgeClass = (score) => {
  if (score >= 5000) return 'bg-purple-100 text-purple-800'
  if (score >= 3000) return 'bg-yellow-100 text-yellow-800'
  if (score >= 2000) return 'bg-gray-100 text-gray-800'
  return 'bg-gray-100 text-gray-800'
}

const logout = () => {
  currentUser.value = null
  router.push('/login')
}

onMounted(() => {
  if (window.feather) {
    window.feather.replace()
  }
})
</script>