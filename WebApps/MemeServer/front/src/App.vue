<template>
  <div class="min-h-screen">
    <Navigation :user="currentUser" @logout="handleLogout" />
    <HeroSection @start-quiz="handleStartQuiz" />
    <CategoriesSection @select-category="handleCategorySelect" />
    <QuizSection
      v-if="showQuiz"
      :current-questions="currentQuestions"
      :current-answers="currentAnswers"
      @submit-answer="handleSubmitAnswer"
      @next-question="handleNextQuestion"
    />
    <ResultsSection
      v-if="showResults"
      :results="quizResults"
    />
    <LeaderboardSection />
    <ProfileSection :user="currentUser" />
    <FooterSection />
  </div>
</template>

<script setup>
import { ref, onMounted, reactive } from 'vue';
import Navigation from './components/Navigation.vue';
import HeroSection from './components/HeroSection.vue';
import CategoriesSection from './components/CategoriesSection.vue';
import QuizSection from './components/QuizSection.vue';
import ResultsSection from './components/ResultsSection.vue';
import LeaderboardSection from './components/LeaderboardSection.vue';
import ProfileSection from './components/ProfileSection.vue';
import FooterSection from './components/FooterSection.vue';

// 状态管理
const currentUser = ref(null);
const showQuiz = ref(false);
const showResults = ref(false);
const currentQuestions = ref([]);
const currentAnswers = ref([]);
const quizResults = ref({});

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

// 事件处理函数
const handleStartQuiz = () => {
  console.log('开始测验');
};

const handleCategorySelect = async (category) => {
  try {
    const questions = await loadQuestions(category);
    currentQuestions.value = questions;
    showQuiz.value = true;
    // 滚动到测验区域
    document.querySelector('.gradient-bg')?.scrollIntoView({ behavior: 'smooth' });
  } catch (error) {
    console.error('加载题目失败:', error);
  }
};

const handleSubmitAnswer = (answer) => {
  currentAnswers.value = [answer];
};

const handleNextQuestion = async () => {
  if (currentAnswers.value.length === 0) {
    alert('请先选择答案');
    return;
  }
  try {
    const results = await submitTest(currentAnswers.value);
    quizResults.value = results;
    showQuiz.value = false;
    showResults.value = true;
    // 滚动到结果区域
    document.getElementById('results-section')?.scrollIntoView({ behavior: 'smooth' });
  } catch (error) {
    console.error('提交失败:', error);
  }
};

const handleLogout = async () => {
  try {
    await apiRequest('/auth/logout', { method: 'POST' });
    currentUser.value = null;
  } catch (error) {
    console.error('登出失败:', error);
  }
};

// 测试相关函数
const loadQuestions = async (category = '', count = 10) => {
  try {
    const params = new URLSearchParams();
    if (category) params.append('category', category);
    params.append('count', count);
    const data = await apiRequest(`/test/questions?${params}`);
    return data.questions;
  } catch (error) {
    alert('加载题目失败: ' + error.message);
    throw error;
  }
};

const submitTest = async (answers) => {
  try {
    const data = await apiRequest('/test/submit', {
      method: 'POST',
      body: JSON.stringify({ answers })
    });
    return data;
  } catch (error) {
    alert('提交测试失败: ' + error.message);
    throw error;
  }
};

// 页面初始化
onMounted(() => {
  // 初始化AOS动画和Feather图标
  if (window.AOS) {
    window.AOS.init();
  }
  if (window.feather) {
    window.feather.replace();
  }
  
  // 加载用户资料
  loadUserProfile();
});

// 用户相关函数
const loadUserProfile = async () => {
  try {
    const data = await apiRequest('/user/profile');
    currentUser.value = data;
  } catch (error) {
    console.error('加载用户资料失败:', error);
  }
};
</script>

<style scoped>
/* 可以将全局样式移到单独的 CSS 文件 */
</style>